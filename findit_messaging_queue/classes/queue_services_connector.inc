<?php

module_load_include('inc', 'findit_messaging_queue', 'classes/queue.controller');
module_load_include('inc', 'findit_messaging_queue', 'findit_messaging_queue');
module_load_include('inc', 'findit_merchant_services', 'classes/finditfactory.classes');

/**
 * @file
 * Classes to interect with rest api and message queue
 */
class QueueServicesConnector implements finditQueueInterface {

  /**
   * @param type $data
   * $data will be array of arguments
   */
  public function queuereceivercallback($data, $apikey) {
    $options = array(
      'method' => 'POST',
      'data' => "data=" . $data->body . '&op=' . $data->properties['message_id'],
      'timeout' => 15,
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );
    $url = $this->services_get($apikey);
    drupal_http_request($url, $options);
  }

  /**
   * @param type $data
   * @param type $op
   */
  public function queueapiinvoke($data, $op) {
    $options = array(
      'method' => 'POST',
      'data' => $data,
      'timeout' => 15,
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );
    $url = $this->services_get('merchant_sender');
    drupal_http_request($url, $options);
  }

  /**
   * @return type
   * $key will be string with key name
   */
  public function services_get($key) {
    $query = db_select('findit_services_settings', 'fs');
    $query->fields('fs', array('service_url'))
        ->condition('fs.service_name', $key);
    $result = $query->execute()->fetchField();
    return $result;
  }

  public function sendtoqueue($data) {
    $op = $data['op'];
    $postdata = drupal_json_decode($data['data']);
    $type = $postdata['type'];

    $message_id = $op;
    $local = variable_get('localdev', FALSE);
    if ($local) {
      $serviceUrl = variable_get('marketsenderurl');
      $options = array(
        'method' => 'POST',
        'data' => $data['data'],
        'timeout' => 15,
        'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
        'op' => $message_id,
      );
      $curl = curl_init($serviceUrl);
      curl_setopt($curl, CURLOPT_RETURNTRANSFER, true);
      curl_setopt($curl, CURLOPT_POST, true);
      curl_setopt($curl, CURLOPT_POSTFIELDS, http_build_query($options));
      $curl_response = curl_exec($curl);
      curl_close($curl);
    }
    else {
      $sender = new MessageSender();
      $sender->execute($data['data'], 'findit_merchant_place_to_market', $message_id);
    }
    watchdog('Findit_send_message_to_rabbit_message_queue', print_r($data, true));
    return $postdata;
  }

  public function queueapireceiver($data) {
    $postdata = drupal_json_decode($data['data']);
    $type = $postdata['type'];
    $op = $data['op'];
    switch ($op) {
      case 'create':
        $options = array(
          'method' => 'POST',
          'data' => "data=" . $data['data'],
          'timeout' => 15,
          'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
        );
        $order = new FinditFactory($type . '_' . $data['op'], $options);
        break;
      case 'update':
        $options = array(
          'method' => 'PUT',
          'data' => "data=" . $data['data'],
          'timeout' => 15,
          'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
        );
        $service_key = $type . '_' . $data['op'];
        $receiver_id = $postdata['refrence_order_id'];
        $url = self::services_get($service_key) . '/' . $receiver_id;
        dd($options);
        $order = new FinditFactory($type . '_' . $data['op'], $options, $url);
        break;
      case 'delete':
        reurn;
        break;
    }
  }

  public function queueaccess() {
    return true;
  }

}

<?php

module_load_include('inc', 'findit_merchant_services', 'classes/services.controller');

/**
 * @file
 * Class related to order processing
 */
class findit_order_return implements finditServicesInterface {

  /**
   * Creates orders
   * @param type $method
   * @return type
   */
  public function create($method) {
    $data = drupal_json_decode($method['data']);
    watchdog('data_received', var_export($data, 1));
    $type = $data['type'];
    $user = $data['user'];
    $shipping_address = $data['shiping_address'];
    //$billing_address = $data['billing_address'];
    $return_id = $data['return_id'];
    $commerce_order_total = $data['return_total'];
    if ($type == "commerce_return") {
      $order = commerce_order_load($data['order_ref']);
      //$order = order_load()
      $return = commerce_return_new($order, 'pending');
      $return->author_webservice = 1;
      unset($return->commerce_return_line_items['und']);
      $unique_store_id = self::commerce_order_get_id_by_store($order->commerce_store[LANGUAGE_NONE]['0']['target_id']);
      foreach ($data['line_item'] as $key => $value) {
        $product = commerce_product_load_by_sku($value['sku']);
        //$amount = self::commerce_order_get_price_by_association($value['sku'], $unique_store_id);
        $amount = findit_order_return_get_lineitem_cost($data['order_ref']); // get theprice for return item
        $product->commerce_price[LANGUAGE_NONE]['0']['amount'] = $amount;

        $line_item = commerce_product_line_item_new($product, $value['quantity'], 0);
        //TODO map tid on different server.
        $line_item->data = $value['data'];
        commerce_line_item_save($line_item);
        $return->commerce_return_line_items['und'][] = array('line_item_id' => $line_item->line_item_id);
      }
      $files = array();
      foreach ($data['images'] as $key => $value) {
        if (function_exists('remote_stream_wrapper_file_load_by_uri')) {
          $file = remote_stream_wrapper_file_load_by_uri($value);
          if (!$file) {
            $file = remote_stream_wrapper_file_create_by_uri($value);
            $file->status = FALSE;
            file_save($file);
            $files[] = (array) $file;
          }
        }
      }
      if (!empty($files)) {
        $return->field_image_return[LANGUAGE_NONE] = $files;
      }
      $return->commerce_order_reference[LANGUAGE_NONE]['0']['target_id'] = $order->order_id;
      $return->commerce_return_total = $commerce_order_total;
      $return->field_return_ref[LANGUAGE_NONE]['0']['value'] = $return_id;
      $return->field_user_refrence[LANGUAGE_NONE]['0']['value'] = $user;
      $return->field_return_address = $shipping_address;
      try {
        commerce_return_save($return);
      }
      catch (Exception $e) {
        watchdog('order_return_service', 'Error: !message', array('!message' => $e->getMessage()), WATCHDOG_ERROR);
        return 'ERROR: ' . $e->getMessage();
      }
      return $return;
    }
  }

  /**
   * Retrieve data from services
   */
  public function retrieve() {
    return 'i m here';
  }

  /**
   * @param $ids
   *   An array of entity IDs.
   */
  public function update($id, $method) {
    $data = drupal_json_decode($method['data']);
    $type = $data['type'];
    $user = $data['user'];

    $return_id = $data['return_id'];

    if ($type == "commerce_return") {
      if ($data['return_ref'] != 0) {
        $return = commerce_return_load($data['return_ref']);
      }
      else {
        $query = db_select('field_data_field_return_ref', 'cor')
            ->fields('cor', array('entity_id'))
            ->condition('field_return_ref_value', $data['return_id'])
            ->condition('bundle', 'commerce_return');
        $result = $query->execute()->fetchAssoc();
        $id = $result['entity_id'];
        $return = commerce_return_load($id);
        $return->field_return_ref[LANGUAGE_NONE]['0']['value'] = $data['return_id'];
      }
      if (isset($return)) {
        $return->author_webservice = 1;
        commerce_return_status_update($return, $data['status'], FALSE, TRUE, t('Status updated from marketplace on !time.', array(
          '!time' => date('c')
        )));
      }
    }
    return isset($return) ? $return : '';
  }

  /**
   * Delete permanently saved entities.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $ids
   *   An array of entity IDs.
   */
  public function delete() {
    return;
  }

  /**
   * Check if a current user has access
   * @param expects logged user id
   * @return True/False
   */
  public function access() {
    return TRUE;
  }

  public function commerce_order_get_price_by_association($sku, $store_unique_id) {
    $result = db_select('eck_association', 'ea')
        ->fields('ea', array('price'))
        ->condition('ea.sku', $sku)
        ->condition('ea.unique_store_id', $store_unique_id)
        ->execute()
        ->fetchField();
    return $result;
  }

  public static function commerce_order_get_id_by_store($store_id) {
    $result = db_select('commerce_store', 'cs')
        ->fields('cs', array('unique_store_id'))
        ->condition('cs.store_id', $store_id)
        ->execute()
        ->fetchField();
    return $result;
  }

}

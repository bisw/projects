<?php

module_load_include('inc', 'findit_marketplace_services', 'classes/services.controller');
module_load_include('inc', 'commerce', 'includes/commerce.controller.inc');

/**
 * @file
 * Class related to order processing
 */
class findit_commerce_order implements finditServicesInterface {

  public function create($method) {
    $data = drupal_json_decode($method['data']);
    $type = $data['type'];
    $user = $data['user'];
    $shipping_address = $data['shiping_address'];
    $billing_address = $data['billing_address'];
    $order_id = $data['order_id'];
    if ($type == "commerce_order") {
      $order = commerce_order_new($user, 'pending');
      commerce_order_save($order);
      $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
      foreach ($data['line_item'] as $key => $value) {
        commerce_line_item_new();
        $product = commerce_product_load($value['product_id']);
        $line_item = commerce_product_line_item_new($product, $value['quantity'], $order->order_id);
        commerce_line_item_save($line_item);
        $order_wrapper->commerce_line_items[] = $line_item;
        $order->field_order_refrence[LANGUAGE_NONE]['0']['value'] = $order_id;
        $order->field_user_refrence[LANGUAGE_NONE]['0']['value'] = $user;
        $order->field_shipping_address = $shipping_address;
        $order->field_billing_address = $billing_address;
      }
      $order->commerce_store = drupal_json_decode($data['store']);
      commerce_order_save($order);
      return $order;
    }
  }

  /**
   * Retrieve data from services
   */
  public function retrieve($id) {
    return 'i m here';
  }

  /**
   * @param $ids
   *   An array of entity IDs.
   */
  public function update($id, $data) {
    sleep(5);
    $data = drupal_json_decode($data['data']);
    watchdog("update_log", print_r($data, true));
    $order = commerce_order_load($id);
    $order_id = $data['order_id'];
    $unique_store_id = $data['unique_store_id'];
    $store = self::commerce_order_get_store_by_id($unique_store_id);
    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $order->field_order_refrence[LANGUAGE_NONE]['0']['value'] = $order_id;
    $order->commerce_store[LANGUAGE_NONE]['0']['target_id'] = $store;

    commerce_order_save($order);
    //give store credit to user if return is store_credit
    watchdog('Store_Credit_Get_Update', print_r($order, true));
    if (isset($data['data']['return_store_credit_amount']) && $data['status'] == "store_credit_completed") {
      $amount = 1.0 * $data['data']['return_store_credit_amount'];
      //commerce_store_credit_add_credit_add($order, $amount, TRUE);
      $order_number = _get_fany_number_by_order_group($order->order_group);
      $reason = t("Refunded on Order @number", array('@number' => $order_number));
      commerce_store_credit_add_credit($order->uid, $amount, $reason, $order->order_group);
    };
    $log = $data['log'];
    $status = $data['status'];
    commerce_order_status_update($order, $status, FALSE, TRUE, $log);
    return t("Order Updated");
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
    return true;
  }

  /**
   * 
   * @param type $store_uniq_id
   */
  public static function commerce_order_get_store_by_id($store_uniq_id) {
    $result = db_select('commerce_store', 'cs')
        ->fields('cs', array('store_id'))
        ->condition('cs.unique_store_id', $store_uniq_id)
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

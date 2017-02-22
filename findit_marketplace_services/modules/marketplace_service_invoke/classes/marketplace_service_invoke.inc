<?php

/**
 * @file
 * Send data to queue.
 */
module_load_include('inc', 'findit_messaging_queue', 'classes/queue_services_connector');
module_load_include('inc', 'findit_messaging_queue', 'classes/queue.controller');

class SendOrderToQueue {

  /**
   * Creates orders.
   *
   * @param type $method
   * @return type
   */
  public function create($data) {
    $order = (object) $data;
    $payment_cod = FALSE;
    $order->field_order_refrence[LANGUAGE_NONE][0]['value'] = rand();
    $op = "create";
    $payment_cod = check_order_is_by_cod($order->data);
    $type = $order->type;
    $user = $order->uid;
    $order_id = $order->order_id;
    $order_number = $order->order_number;
    $order_group = $order->order_group;
    $billing_address_id = $order->commerce_customer_billing[LANGUAGE_NONE]['0']['profile_id'];
    $shipping_address_id = $order->commerce_customer_shipping[LANGUAGE_NONE]['0']['profile_id'];
    $shipping_address_load = commerce_customer_profile_load($shipping_address_id);
    $billing_address_load = commerce_customer_profile_load($billing_address_id);
    $billing_address = $billing_address_load->commerce_customer_address;
    $shipping_address = $shipping_address_load->commerce_customer_address;
    $order_total = $order->commerce_order_total;
    $uniqe_store_id = $order->field_store_unique_id[LANGUAGE_NONE]['0']['value'];
    if (!empty($order->field_combo_entity)) {
      $comboid = $order->field_combo_entity[LANGUAGE_NONE][0]['target_id'];
    }
    else {
      if (!empty($order->field_combo_id)) {
        $comboid = $order->field_combo_id[LANGUAGE_NONE][0]['target_id'];
      }
    }
    $shipping_address_wrapper = entity_metadata_wrapper('commerce_customer_profile', $shipping_address_load);
    $shippment_adress_value = $shipping_address_wrapper->commerce_customer_address->value();
    $phone_number = $shippment_adress_value['mobile_number'];
    /* First name in Order */
    $first_name = $shippment_adress_value['first_name'];
    $last_name = $shippment_adress_value['last_name'];
    if (!empty($order->field_first_name)) {
      $first_name = $order->field_first_name[LANGUAGE_NONE][0]['value'];
    }
    $order_count = count(findit_get_current_group_orders($order_group));
    foreach ($order->commerce_line_items as $key => $value) {
      foreach ($value as $lid) {
        $line_ite_load = commerce_line_item_load($lid['line_item_id']);
        $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_ite_load);
        if (isset($line_ite_load->commerce_product[LANGUAGE_NONE]['0']['product_id'])) {
          $line_item_array[] = array(
            "line_item_id" => $line_ite_load->line_item_label,
            "uniqe_store_id" => $uniqe_store_id,
            "quantity" => $line_ite_load->quantity,
            "type" => $line_ite_load->type,
            "product_unit_price" => $line_ite_load->commerce_unit_price[LANGUAGE_NONE][0]['amount'],
            "product_total_price" => $line_ite_load->commerce_total[LANGUAGE_NONE][0]['amount'],
            "discount_amt" => $line_ite_load->field_combo_discount_amount[LANGUAGE_NONE][0]['value'],
          );
        }
        elseif (isset($line_ite_load->commerce_shipping_service[LANGUAGE_NONE]['0']['value'])) {
          $line_item_array[] = array(
            "line_item_id" => $line_ite_load->commerce_shipping_service[LANGUAGE_NONE]['0']['value'],
            "quantity" => $line_ite_load->quantity,
            "type" => $line_ite_load->type,
          );
        }
        elseif ($line_ite_load->type == 'commerce_discount') {
          if ($line_item_wrapper->field_combo_id->value()) {
            $line_item_array[] = array(
              "type" => $line_ite_load->type,
              "line_item_id" => $line_ite_load->line_item_id,
              "discount_label" => $line_ite_load->line_item_label,
              "discount_amt" => $line_ite_load->commerce_unit_price[LANGUAGE_NONE][0]['amount'],
              "discount_total" => $line_ite_load->commerce_total[LANGUAGE_NONE][0]['amount'],
              "discount_combo_id" => $line_ite_load->field_combo_id[LANGUAGE_NONE][0]['value'],
            );
          }
          elseif ($line_ite_load->field_type[LANGUAGE_NONE][0]['value'] == 'findit_store_credit') {
            $line_item_array[] = array(
              "type" => $line_ite_load->type,
              "line_item_id" => $line_ite_load->line_item_id,
              "discount_label" => $line_ite_load->line_item_label,
              "discount_amt" => $line_ite_load->commerce_unit_price[LANGUAGE_NONE][0]['amount'],
              "discount_total" => $line_ite_load->commerce_total[LANGUAGE_NONE][0]['amount'],
              "discount_type" => 'findit_store_credit',
            );
          }
          else {
            $line_item_array[] = array(
              "type" => $line_ite_load->type,
              "line_item_id" => $line_ite_load->line_item_id,
              "discount_label" => $line_ite_load->data['discount_name'],
            );
          }
        }
        elseif ($line_ite_load->type == "commerce_coupon") {
          $coupon_wrapper = entity_metadata_wrapper('commerce_coupon', $line_item_wrapper->commerce_coupon_reference->value());
          $line_item_array[] = array(
            "type" => $line_ite_load->type,
            "line_item_id" => $line_ite_load->line_item_id,
            "coupon_code" => $coupon_wrapper->commerce_coupon_code->value(),
          );
        }
      }

      $orderDetail = drupal_json_encode(array(
        'type' => $type,
        'status' => $order->status,
        'order_id' => $order_id,
        'order_group' => $order_group,
        'order_number' => $order_number,
        'user' => $user,
        'mail' => $order->mail,
        'line_item' => $line_item_array,
        'order_total' => $order_total,
        'uniqe_store_id' => $uniqe_store_id,
        'shiping_address' => $shipping_address,
        'billing_address' => $billing_address,
        'first_name' => $first_name,
        'phone_number' => $phone_number,
        'order_data' => $order->data,
        'order_count' => $order_count,
        'payment_cod' => $payment_cod,
        'combo_id' => isset($comboid) ? $comboid : 0
      ));
      $orderDetail = 'data=' . $orderDetail . '&op=create';
      try {
        $service = new QueueServicesConnector;
        $service->queueapiinvoke($orderDetail, 'create');
      }
      catch (Exception $ex) {
        watchdog('send_order_to_merchant', $ex->getMessage());
      }
    }
  }

  /**
   * Check if a current user has access
   * @param expects logged user id
   * @return True/False
   */
  public function access() {
    return TRUE;
  }

}

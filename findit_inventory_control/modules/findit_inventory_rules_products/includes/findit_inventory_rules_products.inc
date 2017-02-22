<?php

/**
 * @file
 *  Include file for inventory rules for products.
 */

/**
 * Form for creating inventory rules for product.
 */
function findit_inventory_rule_product_form($form, &$form_state, $irp_id = NULL) {
  if ($irp_id) {
    $product_inventory_rule_item = entity_load_single('findit_product_inventory_rule', $irp_id);
    $form['irp_id'] = array(
      "#type" => "value",
      "#value" => $irp_id,
    );
  }
  $form['product_sku'] = array(
    '#type' => 'textfield',
    '#size' => 25,
    '#required' => TRUE,
    '#attributes' => array('placeholder' => t('Search for SKU id, Item Title')),
    '#autocomplete_path' => 'commerce_product/autocomplete/commerce_product/0/0',
    // '#default_value' => isset($_GET['term']) ? base64_decode($_GET['term']) : '',
    "#default_value" => isset($product_inventory_rule_item->sku) ? $product_inventory_rule_item->sku : ""
  );
  $form['inventory_soft_limit'] = array(
    "#type" => "textfield",
    "#title" => t("Inventory Soft Limit"),
    "#default_value" => isset($product_inventory_rule_item->soft_limit) ? $product_inventory_rule_item->soft_limit : ""
  );
  $form['inventory_hard_limit'] = array(
    "#type" => "textfield",
    "#title" => t("Inventory Hard Limit"),
    "#default_value" => isset($product_inventory_rule_item->hard_limit) ? $product_inventory_rule_item->hard_limit : ""
  );
  $form['inventory_email_group'] = array(
    '#type' => "select",
    '#title' => t("Inventory Email Group"),
    // '#autocomplete_path' => 'findit-inventory-rule-email',
    '#options' => findit_inventory_mail_groups(),
    "#description" => t("Please enter product inventory rule email group"),
    '#default_value' => isset($product_inventory_rule_item->email_group) ? $product_inventory_rule_item->email_group : ""
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Cancel'), 'inventory-rule-product', array('attributes' => array('class' => array('inventory-rules-cancel')))),
  );
  return $form;
}

/**
 * Validation function for creating inventory rules for product.
 */
function findit_inventory_rule_product_form_validate($form, &$form_state) {
  $validate_sku = TRUE;
  if (isset($form_state['values']['irp_id'])) {
    // On edit form if sku has not been changed then do not validate for
    // duplicacy of sku.
    $sku = findi_get_sku_from_inventory_rule_product_id($form_state['values']['irp_id']);
    if ($sku == $form_state['values']['product_sku']) {
      $validate_sku = FALSE;
    }
  }
  if ($validate_sku == TRUE) {
    // Check for duplicacy of poduct id.
    $query = db_select('findit_products_inventory_rule');
    $query->condition('sku', $form_state['values']['product_sku']);
    $query->fields('findit_products_inventory_rule', array('id'));
    $result = $query->execute()->fetchAssoc();
    if (!empty($result)) {
      form_set_error('product_sku', t('Inventory Rule for the selected product already exists.'));
    }
  }
  // Check if atleast one value is set.
  if (empty($form_state['values']['inventory_soft_limit']) && empty($form_state['values']['inventory_hard_limit']) && empty($form_state['values']['inventory_email_group'])) {
    form_set_error('inventory_soft_limit', t('Please specify at least one value.'));
    form_set_error('inventory_hard_limit', '');
    form_set_error('inventory_email_group', '');
  }
}

/**
 * Submit function for setting inventory rule for products.
 */
function findit_inventory_rule_product_form_submit($form, &$form_state) {
  $inventory_rule_product = findit_inventory_rules_products_initialize();
  $values = $form_state['values'];
  if (isset($values['irp_id'])) {
    $inventory_rule_product = entity_load_single('findit_product_inventory_rule', $values['irp_id']);
  }
  $inventory_rule_product->created = isset($inventory_rule_product->created) ? $inventory_rule_product->created : time();
  $inventory_rule_product->changed = time();
  $inventory_rule_product->soft_limit = $values['inventory_soft_limit'];
  $inventory_rule_product->hard_limit = $values['inventory_hard_limit'];
  $inventory_rule_product->sku = $values['product_sku'];
  $inventory_rule_product->email_group = $values['inventory_email_group'];
  $inventory_rule_product->is_new_revision = TRUE;
  entity_save('findit_product_inventory_rule', $inventory_rule_product);

  // Redirect the form to listing page after submit.
  $form_state['redirect'] = 'inventory-rule-product';
}

/**
 * Delete callback for inventory rule for product.
 */
function findit_inventory_rule_product_delete_form($form, &$form_state, $irp_id) {
  $form['irp_id'] = array(
    '#type' => 'value',
    '#value' => $irp_id,
  );
  return confirm_form($form, t('Are you sure you want to delete this inventory rule for the product?'), 'findit_product_inventory_rule');
}

/**
 * Submit callback to delete word from abusive word list.
 */
function findit_inventory_rule_product_delete_form_submit($form, &$form_state) {
  if ($form_state['values']['irp_id']) {
    entity_delete('findit_product_inventory_rule', $form_state['values']['irp_id']);
  }
  drupal_set_message(t('Successfully Deleted Inventory Rule for the Poduct'));
  $form_state['redirect'] = 'inventory-rule-product';
}

/**
 * Callback function for listing of inventory rules for products.
 */
function findit_inventory_rules_products_list() {
  global $user;
  $build = array();
  $build['back_link'] = array(
    '#markup' => '<div class="inventory-rule-product-back-link">' . l(t('BACK'), 'user/' . $user->uid . '/inventory-rules') . '</div>',
  );
  $header = array(
    array('data' => t('SKU'), 'field' => 'sku', 'sort' => 'asc'),
    array('data' => t('Soft Limit'), 'field' => 'soft_limit', 'sort' => 'asc'),
    array('data' => t('Hard Limit'), 'field' => 'hard_limit', 'sort' => 'asc'),
    array('data' => t('Email Group'), 'field' => 'email_group', 'sort' => 'asc'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $inventory_rules_poducts = db_select('findit_products_inventory_rule', 'pir')
          ->fields('pir')
          ->extend('PagerDefault')
          ->extend('TableSort')
          ->orderByHeader($header)
          ->limit(10)
          ->execute()->fetchAll();
  $build['add_link'] = array(
    '#markup' => '<div class="add-inventory-rule-product-link">' . l(t('Add Inventory Rule for Products'), 'create/inventory-rule-product') . '</div>',
  );
  if (!empty($inventory_rules_poducts)) {
    $rows = array();

    foreach ($inventory_rules_poducts as $inventory_rule) {
      $rows[] = array(
        $inventory_rule->sku,
        $inventory_rule->soft_limit,
        $inventory_rule->hard_limit,
        $inventory_rule->email_group,
        l(t('Edit'), 'inventory-rule-product/' . $inventory_rule->id . '/edit'),
        l(t('Delete'), 'inventory-rule-product/' . $inventory_rule->id . '/delete'),
      );
    }
    $build['table'] = array(
      '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
    );
    $build['pager'] = array(
      '#markup' => theme('pager'),
    );
  }
  else {
    $build['no_result'] = array(
      '#markup' => '<div>' . t('No results found.') . '</div>',
    );
  }
  return $build;
}

/**
 * Function for getting sku from inventory rule for product entity id.
 */
function findi_get_sku_from_inventory_rule_product_id($irp_id) {
  $query = db_select('findit_products_inventory_rule');
  $query->condition('id', $irp_id);
  $query->fields('findit_products_inventory_rule', array('sku'));
  $result = $query->execute()->fetchAssoc();
  return $result['sku'];
}

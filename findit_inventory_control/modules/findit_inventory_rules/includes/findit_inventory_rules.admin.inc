<?php

/**
 * @file
 * Include file for findit_inventory_rules module.
 */

/**
 * Callback function for global inventory rule form.
 */
function findit_inventory_update_global($form, &$form_state) {
  global $user;
  $form['back_link'] = array(
    '#markup' => '<div class="inventory-rule-product-back-link">' . l(t('BACK'), 'user/' . $user->uid . '/inventory-rules') . '</div>',
  );
  $form['findit_global_inventory_rule_soft_limit'] = array(
    '#type' => "textfield",
    '#title' => t("Global Inventory Rule Soft Limit"),
    "#description" => t("Please enter global inventory rule soft limit for all categories (expected orerriden)"),
    '#default_value' => variable_get('findit_global_inventory_rule_soft_limit', 0),
  );
  $form['findit_global_inventory_rule_hard_limit'] = array(
    '#type' => "textfield",
    '#title' => t("Global Inventory Rule Hard Limit"),
    "#description" => t("Please enter global inventory rule hard limit for all categories (expected orerriden)"),
    '#default_value' => variable_get('findit_global_inventory_rule_hard_limit', 0),
  );
  $form['findit_global_inventory_rule_email_group'] = array(
    '#type' => "select",
    '#title' => t("Global Email group"),
    // '#autocomplete_path' => 'findit-inventory-rule-email',
    '#options' => findit_inventory_mail_groups(),
    "#description" => t("Please enter global email group for all categories (expected orerriden)"),
    '#default_value' => variable_get('findit_global_inventory_rule_email_group', ''),
  );
  //@TODO - Reset all
  /* $form['reset'] = array(
    '#type' => 'checkbox',
    '#title' => t('Reset ALL'),
    '#return_value' => 1,
    '#description' => t('Warning: This operation cannot be undone.'),

    ); */
  $form['#submit'] = array('findit_inventory_update_global_submit');
  return system_settings_form($form);
}

/**
 * Submit function for global update of inventory rule.
 */
function findit_inventory_update_global_submit($form, &$form_state) {
  $vocabulary = taxonomy_vocabulary_machine_name_load(FINDIT_PRODUCT_CATEGORY);
  // $nested_tree = fintit_taxonomy_get_nested_tree($tree);
  $tree = taxonomy_get_tree($vocabulary->vid, 0, 1);
  $soft_limit = $form_state['values']['findit_global_inventory_rule_soft_limit'];
  $hard_limit = $form_state['values']['findit_global_inventory_rule_hard_limit'];
  $email_group = $form_state['values']['findit_global_inventory_rule_email_group'];
  $global_rule = new stdClass();
  $global_rule->tid = 0;
  array_unshift($tree, $global_rule);
  foreach ($tree as $term) {
    update_inventory_rule($vocabulary->vid, $term->tid, $soft_limit, $hard_limit, $email_group, 0, 0);
  }
}

/**
 * Function for updating inventory rule.
 */
function update_inventory_rule($vid, $tid, $soft_limit = 0, $hard_limit = 0, $email_group = '', $is_dirty = 0, $force_reset = 0) {
  global $user;
  if ($force_reset == 0 && $tid != 0) {
    if ($is_dirty == 0 && is_inventory_rule_dirty($tid)) {
      return;
    }
  }
  $data = array(
    'inventory_rule_soft_limit' => $soft_limit,
    'inventory_rule_hard_limit' => $hard_limit,
    'email_group' => $email_group,
    'tid' => $tid,
    'dirty' => $is_dirty,
    'status' => 1,
    'updated' => time(),
    'updated_by' => $user->uid,
  );

  if (!is_not_inventory_rule_new($tid)) {
    $data['created'] = time();
    $data['created_by'] = $user->uid;
    // Insert new entry.
    $data = drupal_write_record('findit_inventory_rules', $data);
  }
  else {
    // Update existing entry.
    $data = drupal_write_record('findit_inventory_rules', $data, 'tid');
  }

  // Update all the child level categories.
  $tree = taxonomy_get_tree($vid, $tid, 1);
  foreach ($tree as $term) {
    update_inventory_rule($vid, $term->tid, $soft_limit, $hard_limit, $email_group, 0);
  }
}

/**
 * Function for checking if the inventory rule is new for the category.
 */
function is_not_inventory_rule_new($tid) {
  $is_new = db_select('findit_inventory_rules', 'ir')
      ->fields('ir', array('created'))
      ->condition('ir.tid', $tid)
      ->execute()
      ->fetchField();
  return $is_new ? TRUE : FALSE;
}

/**
 * Function for checking if the category is flagged.
 */
function is_inventory_rule_dirty($tid) {
  $is_dirty = db_select('findit_inventory_rules', 'ir')
      ->fields('ir', array('dirty'))
      ->condition('ir.tid', $tid)
      ->execute()
      ->fetchField();
  return $is_dirty ? TRUE : FALSE;
}

/**
 * Callback function for setting findit inventory rules.
 */
function findit_inventory_rules_json() {
  if (isset($_POST['data'])) {
    $vocabulary = taxonomy_vocabulary_machine_name_load(FINDIT_PRODUCT_CATEGORY);
    $data = drupal_json_decode($_POST['data']);

    foreach ($data as $record) {
      $dirty = 1;
      $force_reset = 0;
      if (isset($record['reset']) && $record['reset'] == 1) {
        $force_reset = 1;
        $dirty = 0;
        if ($record['tid'] == 0) {
          $record_data = findit_get_inventory_rule(0);
        }
        else {
          $parents = taxonomy_get_parents($record['tid']);
          $parents = array_keys($parents);
          if (isset($parents[0])) {
            $record_data = findit_get_inventory_rule($parents[0]);
          }
          else {
            $record_data = findit_get_inventory_rule(0);
          }
        }
        $record['soft_limit'] = $record_data['inventory_rule_soft_limit'];
        $record['hard_limit'] = $record_data['inventory_rule_hard_limit'];
        $record['email_group'] = $record_data['email_group'];
        db_delete("findit_inventory_rules_operation")
            ->condition('tid', $record['tid']);
      }
      else {
        update_inventory_rule_operation($record['tid'], $record['soft_limit'], $record['hard_limit'], $record['email_group'], $dirty);
      }
      update_inventory_rule($vocabulary->vid, $record['tid'], $record['soft_limit'], $record['hard_limit'], $record['email_group'], $dirty, $force_reset);
      // Update global variables.
      if ($record['tid'] == 0) {
        variable_set('findit_global_inventory_rule_soft_limit', $record['soft_limit']);
        variable_set('findit_global_inventory_rule_hard_limit', $record['hard_limit']);
        variable_set('findit_global_inventory_rule_email_group', $record['email_group']);
      }
    }
  }
  $tree = findit_get_inventory_tree();
  drupal_json_output($tree);
  drupal_exit();
}

/**
 * Function for getting inventory rules category tree.
 */
function findit_get_inventory_tree() {
  $vocabulary = taxonomy_vocabulary_machine_name_load(FINDIT_PRODUCT_CATEGORY);
  // Get category terms tree.
  $tree = taxonomy_get_tree($vocabulary->vid);
  // Get nested structure of category terms.
  $nested_tree = fintit_taxonomy_get_nested_tree($tree);
  // Get nested structure of category terms with inventory rules.
  $inventory_rules = findit_get_category_tree_inventory_rules();
  $default_inventory_rule = findit_get_inventory_rule();
  $root_tree[] = (object) array(
    'tid' => 0,
    'name' => t('Global'),
    'children' => $nested_tree,
  );
  // Return category tree with inventory rules in json format.
  return findit_parse_tree_inventory_rules($root_tree, $inventory_rules, $default_inventory_rule);
}

/**
 * Function for parsing category tree for inventory rules.
 */
function findit_parse_tree_inventory_rules($tree, $inventory_rules = array(), $default_inventory_rule = array(0, 0, '')) {
  $output = array();
  foreach ($tree as $tid => $term) {
    // Set soft limit and hard limit for each term.
    $soft_limit = isset($inventory_rules[$tid]) ? $inventory_rules[$tid]['soft_limit'] : $default_inventory_rule['inventory_rule_soft_limit'];
    $hard_limit = isset($inventory_rules[$tid]) ? $inventory_rules[$tid]['hard_limit'] : $default_inventory_rule['inventory_rule_hard_limit'];
    $email_group = isset($inventory_rules[$tid]) ? $inventory_rules[$tid]['email_group'] : $default_inventory_rule['email_group'];
    $data = array(
      'tid' => $tid,
      'name' => str_replace('&amp;', '&', $term->name),
      'dirty' => 0,
    );
    if (isset($inventory_rules[$tid]) && $inventory_rules[$tid]['dirty'] == 1) {
      // $data['name'] .= ' (' . $soft_limit . ', ' . $hard_limit . ') *';
      $data['name'] .= ' *';
      $data['dirty'] = 1;
    }
    // else {
    //  $data['name'] .= ' (' . $soft_limit . ', ' . $hard_limit . ')';
    // }
    $data['soft_limit'] = $soft_limit;
    $data['hard_limit'] = $hard_limit;
    $data['email_group'] = $email_group;

    if (isset($term->children)) {
      // Get inventory rules for children.
      $data['children'] = findit_parse_tree_inventory_rules($term->children, $inventory_rules, $default_inventory_rule);
    }
    $output[] = $data;
  }
  return $output;
}

/**
 * Function for updating inventory rules for any category.
 */
function update_inventory_rule_operation($tid, $soft_limit, $hard_limit, $email_group = '', $is_dirty = 0) {
  global $user;
  $data = array(
    'tid' => $tid,
    'dirty' => $is_dirty,
    'email_group' => $email_group,
    'inventory_rule_soft_limit' => $soft_limit,
    'inventory_rule_hard_limit' => $hard_limit,
    'status' => 1,
    'updated' => time(),
    'updated_by' => $user->uid,
  );
  if (!is_not_inventory_rule_new($tid)) {
    $data['created'] = time();
    $data['created_by'] = $user->uid;
    $data = drupal_write_record('findit_inventory_rules_operation', $data);
  }
  else {
    $data = drupal_write_record('findit_inventory_rules_operation', $data, array('tid'));
  }
}

/**
 * Function for getting inventory rules tree.
 */
function findit_get_category_tree_inventory_rules() {
  $inventory_rules = array();
  $result = db_select('findit_inventory_rules', 'ir')
      ->fields('ir', array(
        'tid',
        'inventory_rule_soft_limit',
        'inventory_rule_hard_limit',
        'email_group',
        'dirty',
      ))
      ->execute();
  foreach ($result as $data) {
    $inventory_rules[$data->tid] = array(
      'soft_limit' => $data->inventory_rule_soft_limit,
      'hard_limit' => $data->inventory_rule_hard_limit,
      'email_group' => $data->email_group,
      'dirty' => $data->dirty,
    );
  }
  return $inventory_rules;
}

/**
 * Function for getting default inventory rule.
 */
function findit_get_inventory_rule($tid = 0) {
  $inventory_rule = db_select('findit_inventory_rules', 'ir')
      ->fields('ir', array(
        'inventory_rule_soft_limit',
        'inventory_rule_hard_limit',
        'email_group',
      ))
      ->condition('ir.tid', $tid)
      ->execute()
      ->fetchAssoc();
  if (!$inventory_rule) {
    $inventory_rule['inventory_rule_soft_limit'] = variable_get('findit_global_inventory_rule_soft_limit', 0);
    $inventory_rule['inventory_rule_hard_limit'] = variable_get('findit_global_inventory_rule_hard_limit', 0);
    $inventory_rule['email_group'] = variable_get('findit_global_inventory_rule_email_group', '');
  }
  return $inventory_rule;
}

/**
 * Callback function for returning email id groups in json format.
 */
function findit_inventory_mail_groups_json() {
  $groups = findit_inventory_mail_groups();
  drupal_json_output($groups);
  drupal_exit();
}

/**
 * Callback function for inventory rules interface.
 */
function findit_inventory_rules_interface($read_only = FALSE) {
  drupal_add_js(drupal_get_path('module', 'findit_commissions') . '/assests/tree.jquery.js');
  drupal_add_css(drupal_get_path('module', 'findit_commissions') . '/assests/jqtree.css');
  drupal_add_js(drupal_get_path('module', 'findit_inventory_control') . '/inventory_control_tree.js', array('scope' => 'footer'));

  $email_groups = findit_inventory_mail_groups();
  return theme('findit_inventory_rules_interface', array('data' => $email_groups));
}

/**
 * Form for creating payment rules.
 */
function findit_default_seller_acl_search_form($form, &$form_state) {

  $form['list_link'] = array(
    '#type' => 'markup',
    '#markup' => l(t('Back'), 'default/seller/list') . '<br></br>',
  );
  global $language;
  $product = NULL;
  if (arg(3) != NULL) {
    $sku = arg(3);
    $product = commerce_product_load_by_sku($sku);
    $default_seller = _get_default_seller_by_sku($sku);
  }
  if (isset($product->sku)) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Reset'),
    );
    $sellers = _get_sellers_by_sku($sku);

    $header = array(
      'title' => array('data' => t('Store Name')),
      'mrp' => array('data' => t('MRP')),
      'price' => array('data' => t('Seller Price')),
      'stock' => t('Quantity'),
      'action' => array('data' => t('Action')),
    );
    if (count($sellers)) {
      foreach ($sellers as $seller) {
        $seller_name = _get_seller_name_by_unid($seller->unique_store_id);
        $options[$seller->unique_store_id] = array(
          'title' => $seller_name,
          'mrp' => findit_currency_format($product->commerce_price[LANGUAGE_NONE]['0']['amount'], commerce_default_currency()),
          'price' => t('@price', array('@price' => findit_currency_format($seller->price, commerce_default_currency()))),
          'stock' => $seller->stock,
          'action' => ($seller->unique_store_id == $default_seller) ? t('Default Seller') : ($seller->stock > 0 ? l(t('Set as Default'), '#') : t('Out of Stock')),
//          'action' => ($seller->unique_store_id == $default_seller) ? t('Default Seller') : ($seller->stock > 0 ? l('Set as Default', 'default/set/' . $seller->unique_store_id . '/' . $product->product_id) : t('Out of Stock')),
        );
      }
    }
    if (count($options)) {
      $form['select_seller'] = array(
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $options,
        '#empty' => t('No content available.'),
        '#prefix' => '<div class="set-default-seller">',
        '#suffix' => '</div>',
      );
    }
    else {
      $form['empty_result'] = array(
        '#type' => 'markup',
        '#markup' => '<b><br><br>' . t("No Seller Found") . '</b><br><br>',
      );
    }
    $form['p_detail'] = array(
      '#type' => 'markup',
      '#markup' => _get_product_summary($product),
    );
  }
  else {

    $form['sku_search'] = array(
      '#type' => 'textfield',
      '#size' => 25,
      '#attributes' => array('placeholder' => t('Search for SKU id, Item Title')),
      '#autocomplete_path' => 'sku/autocomplete/sku',
      '#default_value' => arg(3) ? $sku : "",
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Go'),
    );
  }
  return $form;
}

/**
 * Submit function for payment rules form.
 */
function findit_default_seller_acl_search_form_submit($form, &$form_state) {
  $sku = $form_state['values']['sku_search'];
  drupal_goto('product-seller/acl/search/' . $sku);
}

/**
 * Batch process function for sending email for inventory category rules.
 */
//function findit_set_inventory_cat_mail($categories, $from, $pet, $type) {
//  $params = array();
//  $params['pet_from'] = $from;
//  foreach ($categories as $category) {
//    $inventory_rules = findit_get_inventory_rules_category($category);
//    // p($inventory_rules);
//    $inventory_count = findit_products_inventory_count($category, $inventory_rules[$type]);
//    if (!empty($inventory_count)) {
//      $email_ids = findit_inventory_get_email_ids_of_group($inventory_rules['email_group']);
//      $params['pet_to'] = implode(',', $email_ids);
//      $params['inventoy_cat_id'] = $category;
//      $params['inventoy_soft_limit'] = $inventory_rules['inventory_rule_soft_limit'];
//      $params['inventoy_hard_limit'] = $inventory_rules['inventory_rule_hard_limit'];
//      // Send e-mail.
//      pet_send_one_mail($pet, $params);
//    }
//  }
//}

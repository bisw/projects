<?php

/**
 * Callback function to get ssociation list page.
 * @param type $store
 * @param type $type
 * @return type
 */
function findit_merchant_inventory_list($store, $type = 'my') {
  $output = '';
  drupal_set_title('Inventory');
  $globalSearchForm = drupal_get_form('findit_association_globalsearch_form', $store);
  $output .= drupal_render($globalSearchForm);
  if (($type != 'archive') && findit_store_management_check_active_store()) {
    $bulkUploadForm = drupal_get_form('findit_association_bulk_upload', $store);
    $output .= drupal_render($bulkUploadForm);
  }
  $opr = isset($_GET['opr']) && $_GET['opr'] != '' ? $_GET['opr'] : false;
  if (($opr == 'edit') && !findit_store_management_check_active_store()) {
    return drupal_access_denied();
  }
  $inventoryListForm = drupal_get_form('findit_merchant_inventory_list_form', $store, $type);
  $output .= drupal_render($inventoryListForm);
  return $output;
}

/**
 * Callback function to get global search form.
 * @param type $form
 * @param type $form_state
 * @param type $store
 * @return string
 */
function findit_association_globalsearch_form($form, &$form_state, $store) {
  $form = array();
  $form['term'] = array(
    '#type' => 'textfield',
    '#size' => 25,
    '#attributes' => array('placeholder' => t('Search for SKU id, Item Title')),
    '#autocomplete_path' => 'merchant/association/autocomplete/sku',
    '#default_value' => isset($_GET['term']) ? base64_decode($_GET['term']) : '',
  );
  $form['current_path'] = array(
    '#type' => 'hidden',
    '#value' => current_path(),
  );
  
  if (findit_store_management_check_active_store()) {
    $form['store'] = array(
      '#type' => 'markup',
      '#markup' => l('Add Inventory', 'merchant/' . $store->store_id . '/inventory/add', array('attributes' => array('class' => array('btn btn-blue add-inventory')))),
    );
  }
  $form['sku_request'] = array(
    '#type' => 'markup',
    '#markup' => l('Request Product', 'merchant/' . $store->store_id . '/sku/add', array('attributes' => array('class' => array('btn btn-blue add-inventory')))),
  );
//   $form['sku_request_list'] = array(
//    '#type' => 'markup',
//    '#markup' => l('Requested Products List', 'merchant/' . $store->store_id . '/sku_list/', array('attributes' => array('class' => array('btn btn-blue add-inventory')))),
//  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => '<i class="icon-Search" id="globalsearchbtn"></i>',
  );
  return $form;
}

/**
 * Submit handler for global search form.
 * @param type $form
 * @param type $form_state
 */
function findit_association_globalsearch_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $data = array();
  if ($values['term'] != '')
    $data['term'] = base64_encode(trim($values['term']));
  $data = array_filter($data);
  $form_state['redirect'] = array($values['current_path'], array('query' => $data));
}

/**
 * Callback function to get ssociation list form.
 *
 * @global type $user
 * @global type $base_path
 * @global type $pager_total_items
 * @global type $pager_page_array
 * @global type $pager_total
 * @global type $pager_limits
 * @global type $base_url
 * @param type $form
 * @param type $form_state
 * @return array
 */
function findit_merchant_inventory_list_form($form, &$form_state) {
  global $user;
  global $base_path;
  global $pager_total_items, $pager_page_array, $pager_total, $pager_limits;
  global $base_url;

  $opr = isset($_GET['opr']) && $_GET['opr'] != '' ? $_GET['opr'] : false;
  $store = $form_state['build_info']['args'][0];
  $type = $form_state['build_info']['args'][1];
  $header = association_header_new_merchant();

  $associations = array();
  $innerQuery = db_select('eck_association', 'ea');
  $innerQuery->fields('ea');
  $innerQuery->condition('ea.uid', $user->uid);
  $innerQuery->execute();

  $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
  $query->addJoin('LEFT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
  $query->leftjoin('commerce_product_type', 'cpt', 'cpt.type = cp.type');
  $query->leftjoin('field_data_commerce_price', 'fdcp', 'fdcp.entity_id = cp.product_id');
  $query->leftjoin('field_data_catalog_findit', 'fdcf', 'fdcf.entity_id = cp.product_id');
  $query->leftjoin('taxonomy_term_data', 'ttd', 'ttd.tid = fdcf.catalog_findit_tid');
  $query->fields('cp', array('product_id', 'title', 'sku'));
  $query->fields('cpt', array('name'));
  $query->fields('ttd', array('name'));
  $query->fields('fdcp', array('commerce_price_amount', 'commerce_price_currency_code'));
  $query->fields('ea');

  if (isset($_GET['title']) && $_GET['title'] != '') {
    $query->condition('cp.title', '%' . $_GET['title'] . '%', 'like');
  }
  if (isset($_GET['sku']) && $_GET['sku'] != '') {
    $query->condition('cp.sku', trim($_GET['sku']));
  }
  if (isset($_GET['category']) && $_GET['category'] != '') {
    $term = taxonomy_term_load($_GET['category']);
    $term_childrens = array_merge(array($term->tid), array_keys(taxonomy_get_children_all($term->tid, $term->vid)));
    $query->condition('ttd.tid', $term_childrens, 'IN');
  }

  if (isset($_GET['term']) && $_GET['term'] != '') {
    $searchTerm = explode('|', base64_decode($_GET['term']));
    $tsku = reset($searchTerm);
    $or = db_or();
    $or->condition('cp.sku', '%' . trim($tsku) . '%', 'LIKE');
    $or->condition('cp.title', '%' . trim($tsku) . '%', 'LIKE');
    $query->condition($or);
  }

  $query->isNotNull('ea.id');
  switch ($type) {
    case 'all' :
      $query->condition('ea.archive', 0);
      break;
    case 'deplete' :
      $query->where('ea.stock <= ea.stock_low_limit_notification');
      break;
    case 'archive' :
      $query->condition('ea.archive', 1);
      break;
  }

  if (empty($_GET['sort']) && empty($_GET['order'])) {
    $query->orderBy('ea.id', 'desc');
  }
  if (isset($_GET['sort']) && isset($_GET['order']) && ($_GET['order'] == 'Active')) {
    $query->orderBy('ea.statusbyadmin', $_GET['sort']);
  }
  $query->orderByHeader($header);
  $query->limit(25);
  $recordSet = $query->execute()->fetchAll();

  $true = 'Yes';
  $false = 'No';
  $rows = $dump = array();
  $rows['#tree'] = TRUE;
  foreach ($recordSet as $key => $val) {
    $dump[$val->product_id] = $val;
    $row = array();
    $sku_detail_link = '';
    if (isset($val->id) && $val->id > 0) {
      $sku_detail_link = $base_url . '/product/' . $val->sku;
      //$sku_detail_link = sprintf(SKU_DETAIL_URL, $val->product_id, $val->sku);
    } else {
      $sku_detail_link = 'product/' . $val->sku;
    }

    if ($val->id > 0) {
      if ($opr) {
        if (in_array($val->product_id, $_SESSION[ASSO_EDIT_SESS])) {

          $row['sku'] = array('#type' => 'markup',
            '#markup' => sprintf('<div class="sku">%s</div>
                                                            <div class="category">%s (%s)</div>
                                                            <div class="title">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank')))),
          );

          $row['status'] = array('#type' => 'select',
            '#options' => array(1 => t('Yes'), 0 => t('No')),
            '#default_value' => $val->status,
            '#required' => true,
            '#attributes' => array('class' => array('text-center scrollTopOffset'))
          );

          $row['stock_low_limit_notification'] = array(
            '#type' => 'textfield',
            '#element_validate' => array('element_validate_integer'),
            '#default_value' => $val->stock_low_limit_notification ? $val->stock_low_limit_notification : 0,
            '#size' => 11,
            '#attributes' => array('min' => 1, 'max' => 99, 'class' => array('text-center'))
          );

          $row['stock'] = array('#type' => 'textfield',
            '#element_validate' => array('element_validate_integer'),
            '#default_value' => $val->stock ? $val->stock : 0,
            '#size' => 11,
            '#attributes' => array('min' => 1, 'max' => 99999, 'class' => array('text-center'))
          );

          $row['commerce_price_amount']['markup'] = array(
            '#type' => 'markup',
            '#markup' => sprintf('<p class="green text-center">%s</p>', floatval(number_format($val->commerce_price_amount / 100, 2, '.', ''))),
          );

          $row['commerce_price_amount']['value'] = array(
            '#type' => 'hidden',
            '#default_value' => floatval(number_format($val->commerce_price_amount / 100, 2, '.', '')),
          );

          $row['price'] = array('#type' => 'textfield',
            '#element_validate' => array('element_validate_integer'),
            '#default_value' => $val->price ? intval(number_format($val->price, 2, '.', '')) : intval(number_format($val->commerce_price_amount / 100, 2, '.', '')),
            '#size' => 11,
            '#attributes' => array('min' => 1, 'max' => intval(number_format($val->commerce_price_amount / 100, 2, '.', '')), 'class' => array('text-center'))
          );

          $row['note'] = array('#type' => 'fieldset',
            '#collapsible' => FALSE,
            '#collapsed' => FALSE);

          $tmp = explode('||', $val->note);
          $i = 0;
          foreach ($_SESSION['seller_additional_notes'] as $k => $v) {
            if (isset($tmp[$i]) && ($tmp[$i] != '')) {
              $row['note'][$i] = array('#type' => 'checkbox', '#title' => $v, '#default_value' => 1);
            } else {
              $row['note'][$i] = array('#type' => 'checkbox', '#title' => $v,);
            }
            $i++;
          }

          $row['note']['returnable'] = array(
            '#type' => 'checkbox',
            '#default_value' => $val->returnable ? $val->returnable : 0,
            '#title' => 'Return Option',
          );

          $row['note']['gift_wrap'] = array(
            '#type' => 'checkbox',
            '#default_value' => $val->gift_wrap ? $val->gift_wrap : 0,
            '#title' => 'Gift Wrap',
          );
        } else {
          $row['sku'] = array('#type' => 'markup',
            '#markup' => sprintf('<div class="sku">%s</div>
                                                            <div class="category">%s (%s)</div>
                                                            <div class="title">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank')))),
          );


          if ($val->statusbyadmin == 1) {
            $row['status'] = array('#type' => 'markup',
              '#markup' => $val->status > 0 ? $true : $false,
              '#attributes' => array('class' => array('text-center')));
          } else {

            $tval = sprintf('<p class="red">Blocked</p>
                                                 <p class="text-red">Inactive Since: %s</p>
                                                 <ul><li>%s</li></ul>', date('d/m/Y', $val->changed), $val->notebyadmin);

            $row['status'] = array('#type' => 'markup',
              '#markup' => $tval,
              '#attributes' => array('class' => array('text-center')));
          }

          $row['stock_low_limit_notification'] = array('#type' => 'markup',
            '#markup' => $val->stock_low_limit_notification,
            '#attributes' => array('class' => array('text-center')));

          $row['stock'] = array('#type' => 'markup',
            '#markup' => $val->stock,
            '#attributes' => array('class' => array('text-center')));

          $row['commerce_price_amount'] = array('#type' => 'markup',
            '#markup' => sprintf('<p class="green">%s</p>', floatval(number_format($val->commerce_price_amount / 100, 2, '.', ''))),
            '#attributes' => array('class' => array('text-center')));

          $row['price'] = array('#type' => 'markup',
            '#markup' => intval(number_format($val->price, 2, '.', '')),
            '#attributes' => array('class' => array('text-center')));

          $tmp_note = explode('||', $val->note);
          $tmp_note_str = '<ul>';
          foreach ($tmp_note as $k => $v) {
            $tmp_note_str .= sprintf('<li>%s</li>', $v);
          }
          $tmp_note_str .= '</ul>';
          $tmp_note_str .= sprintf('<div class="clearfix"><div class="returnopt %s">Return Option</div>', $val->returnable == 1 ? 'active btn btn-option' : 'disable btn btn-disable');
          $tmp_note_str .= sprintf('<div class="giftwrap %s">Gift Wrap</div></div>', $val->gift_wrap == 1 ? 'active btn btn-option' : 'disable btn btn-disable');

          $row['note'] = array('#type' => 'markup',
            '#markup' => $tmp_note_str,
            '#attributes' => array());
        }
      } else {

        $row['sku'] = sprintf('<div class="sku">%s</div>
                                                        <div class="category">%s (%s)</div>
                                                        <div class="title">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank'))));

        if ($val->statusbyadmin == 1) {
          $row['status'] = array('data' => $val->status > 0 ? $true : $false, 'class' => 'text-center');
        } else {
          $tval = sprintf('<p class="red">Blocked</p>
                                             <p class="text-red">Inactive Since: %s</p>
                                             <ul><li>%s</li></ul>', date('d/m/Y', $val->changed), $val->notebyadmin);
          $row['status'] = array('data' => $tval, 'class' => 'text-center blocked-association', 'id' => $val->product_id);
        }
        $stockCls = ($val->stock < 1) ? 'zero' : 'non-zero';
        $stockLimCls = ($val->stock_low_limit_notification < 1) ? 'zero' : 'non-zero';
        $stockCount = ($val->stock < 1) ? 0 : $val->stock;
        $stockLimtCount = ($val->stock_low_limit_notification < 1) ? 0 : $val->stock_low_limit_notification;

        $row['stock_low_limit_notification'] = array('data' => $stockLimtCount, 'class' => array('text-center', $stockLimCls));
        $row['stock'] = array('data' => $stockCount, 'class' => array('text-center', $stockCls));
        $row['commerce_price_amount'] = array('data' => sprintf('<p class="green text-center">%s</p>', floatval(number_format($val->commerce_price_amount / 100, 2, '.', ''))));
        $row['price'] = array('data' => intval(number_format($val->price, 2, '.', '')), 'class' => 'text-center');

        $tmp_note = isset($val->note) ? explode('||', $val->note) : '';
        $tmp_note_str = '<ul>';
        foreach ($tmp_note as $k => $v) {
          $tmp_note_str .= sprintf('<li>%s</li>', $v);
        }
        $tmp_note_str .= '</ul>';
        $tmp_note_str .= sprintf('<div class="clearfix"><div class="returnopt %s">Return Option</div>', $val->returnable == 1 ? 'active btn btn-option' : 'disable btn btn-disable');
        $tmp_note_str .= sprintf('<div class="giftwrap %s">Gift Wrap</div></div>', $val->gift_wrap == 1 ? 'active btn btn-option' : 'disable btn btn-disable');
        $row['note'] = $tmp_note_str;
      }
      $rows[$val->product_id] = $row;
    }
  }

  drupal_add_js(sprintf('var base_path = "%s"', $base_path), 'inline');
  drupal_add_js(sprintf('var processcart_totalitem = "%d"', isset($_SESSION[ASSO_SESS]) ? count($_SESSION[ASSO_SESS]) : 0), 'inline');

  switch ($opr) {
    case 'edit' :
      $form['products'] = array(
        '#prefix' => getassociationheader($store, $type),
        '#theme' => 'findit_product_association_formtable',
        '#header' => $header,
        '#attributes' => array('id' => 'processcart-table'),
      );

      if (count($rows) > 0) {
        $form['products']['rows'] = $rows;
      } else {
        $form['description']['#markup'] = '<h3>' . t('No match found') . '</h3>';
      }

      break;
    default :
      //Js select all disabled when form is open in edit mode
      $jsSelectAll = $opr ? false : true;
      $form['rows'] = array(
        '#prefix' => getassociationheader($store, $type),
        '#type' => 'tableselect',
        '#header' => $header,
        '#options' => $rows,
        '#default_value' => array(),
        '#attributes' => array(
          'class' => array('tableheader-processed'),
        ),
        '#multiple' => true,
        '#empty' => t('No Record Found.'),
        '#js_select' => $jsSelectAll
      );

      foreach ($recordSet as $key => $val) {
        if ($opr) {
          $form['rows'][$val->product_id]['#disabled'] = true;
        } elseif ($val->statusbyadmin == 0) {
          //$form['rows'][$val->product_id]['#disabled'] = true;
        }
      }
      break;
  }

  $form['back'] = array('#type' => 'hidden', '#default_value' => current_path());
  $form['action'] = array('#type' => 'hidden', '#default_value' => 'edit', '#attributes' => array('id' => array('action')));
  $form['tab'] = array('#type' => 'hidden', '#default_value' => $type);

  $form['store_id'] = array('#type' => 'hidden', '#value' => $store->store_id,);
  $form['unique_store_id'] = array('#type' => 'hidden', '#value' => $store->unique_store_id,);
  $form['dump'] = array('#type' => 'hidden', '#value' => json_encode($dump));

  $queryString = array();
  if (isset($_GET['term']) && $_GET['term'] != '')
    $queryString['term'] = $_GET['term'];
  if (!empty($queryString['term'])) {
    $asso_gl_result = isset($_SESSION['asso_global_search_result'][$store->store_id]) ? $_SESSION['asso_global_search_result'][$store->store_id] : 0;
    $current_apath = current_path();
    $global_result = "<div class=global-search-result>";
    $global_result .= "<span class=global-search-summary>" . t("@total SKUs for @term as per the search term.", array('@total' => $asso_gl_result['count'], '@term' => $asso_gl_result['term'])) . "</span>";
    $global_result .= "<span class=global-search-reset>" . l(t('Reset'), $current_apath) . "</span></div>";

    $form['asso_global_search'] = array(
      "#type" => "markup",
      "#markup" => $global_result,
      "#weight" => -2
    );
  }

  // Get the result count status in bottom.
  $totalCount = isset($pager_total_items[0]) ? $pager_total_items[0] : 0;
  $perPageCount = isset($pager_limits[0]) ? $pager_limits[0] : 0;
  $startCount = $perPageCount * (isset($pager_page_array[0]) ? $pager_page_array[0] : 0);
  $endCount = $startCount + $perPageCount;
  $endCount = $endCount < $totalCount ? $endCount : $totalCount;
  if ($totalCount > 0) {
    $form['current_asso_msg'] = array(
      "#type" => "markup",
      "#markup" => t("Showing @start to @end of @total entries", array('@start' => $startCount, '@end' => $endCount, '@total' => $totalCount)),
    );
  }

  $form['pager']['#markup'] = theme('pager', array('tags' => array()));
  $form['submit']['#weight'] = -1;
  $form['submit']['#prefix'] = '<div class="pull-right archive-wrapper save-cancel-btn">';
  $form['submit']['#suffix'] = '</div>';

  switch ($type) {
    case 'archive' :
      $form['submit'][] = array(array('#attributes' => array('class' => array('btn-blue-border btn icon-Archive-01 btn-archive inventory-archive')), '#type' => 'button', '#value' => 'Un-Archive'));
      // $form['submit'][] = array(array('#attributes' => array('class' => array('btn-blue-border btn icon-Un-Archive-01 btn-unarchive inventory-unarchive')), '#type' => 'button', '#value' => 'UnArchive'),);
      break;
    default :
      if ($opr == 'edit') {
        drupal_add_js(drupal_get_path('module', 'findit_association') . '/webroot/js/findit_association.js');
        $form['submit'][] = array(array('#attributes' => array('class' => array('btn-blue-border btn btn-save back-to-inventory')), '#type' => 'submit', '#value' => 'Save'),
          array('#attributes' => array('class' => array('btn-blue-border btn edit btn-cancel back-to-inventory')), '#type' => 'button', '#value' => 'Cancel'));
      } else {
        $form['submit'][][] = array('#attributes' => array('class' => array('btn-blue-border btn inventory-download btn-download')), '#type' => 'button', '#value' => 'Download');   
        if (findit_store_management_check_active_store()) {
          $form['submit'][][] = array('#attributes' => array('class' => array('btn-blue-border btn icon-Archive-01 btn-archive inventory-archive')), '#type' => 'button', '#value' => 'Archive');
          $form['submit'][][] = array('#attributes' => array('class' => array('btn-blue-border btn icon-Edit btn-edit inventory-edit')), '#type' => 'button', '#value' => 'Edit');
        }
      }
      break;
  }
  return $form;
}

/**
 * Validate association forms.
 * @param type $form
 * @param type $form_state
 */
function findit_merchant_inventory_list_form_validate($form, &$form_state) {
  $data = $form_state['values'];
  $params = drupal_get_query_parameters();
  switch (strtolower($data['action'])) {
    case 'download' :
      $tmp = array();
      if (isset($data['rows']) && array_sum($data['rows']) > 0) {
        foreach ($data['rows'] as $key => $val) {
          if ($val > 0)
            $tmp[$val] = $val;
        }
      }
      $store_id = $data['store_id'];
      $op = 'list';
      $type = $form_state['build_info']['args'][1];
      findit_merchant_download_association_file($tmp, 'csv', $op, $store_id, $type);
      break;
    case 'archive' :
      if (isset($data['rows']) && array_sum($data['rows']) > 0) {

        $tmp = array();
        foreach ($data['rows'] as $key => $val) {
          if ($val > 0)
            $tmp[$val] = $val;
        }

        $query = db_select('eck_association', 'ea');
        $query->fields('ea');
        $query->condition('ea.product', $tmp, 'IN');
        $query->condition('ea.store', $data['store_id']);
        $query->condition('ea.status', 1);
        $resultSet = $query->execute()->fetchAll();
        if (count($resultSet) > 0) {
          form_set_error('global', t('*You can only Archive inactive item'));
        } else {
          $query = db_update('eck_association');
          $query->fields(array('archive' => 1, 'changed' => time()));
          $query->condition('product', $tmp, 'IN');
          $query->condition('store', $data['store_id']);
          $resultSet = $query->execute();

          if ($resultSet) {
            $queryString = array();

            if (isset($_GET['page']))
              $queryString['page'] = $_GET['page'];
            if (isset($_GET['term']))
              $queryString['term'] = $_GET['term'];
            if (!empty($params)) {
              $queryString = array_merge($queryString, $params);
            }
            drupal_goto($data['back'], array('query' => $queryString));
            exit;
          } else {
            form_set_error('global', t('Server has issue please try again!!!'));
          }
        }
      } else {
        form_set_error('global', t('Please select atleast one association'));
      }
      break;
    case 'edit' :
      if (isset($data['rows']) && array_sum($data['rows']) > 0) {
        $tmp = array();
        foreach ($data['rows'] as $key => $val) {
          if ($val > 0)
            $tmp[$key] = $key;
        }
        $_SESSION[ASSO_EDIT_SESS] = $tmp;

        $queryString = array();
        $queryString['opr'] = 'edit';

        if (isset($_GET['page']))
          $queryString['page'] = $_GET['page'];
        if (isset($_GET['term']))
          $queryString['term'] = $_GET['term'];
        if (!empty($params)) {
          $queryString = array_merge($queryString, $params);
        }
        drupal_goto($data['back'], array('query' => $queryString));
        exit;
      } else {
        form_set_error('global', t('Please select atleast one association.'));
      }
      break;
    case 'save' :
      if (isset($form_state['values']['rows']) && !empty($form_state['values']['rows'])) {
        foreach ($form_state['values']['rows'] as $id => $row) {

          if (empty($row['price'])) {
            form_set_error('rows][' . $id . '][price', t('Price is required when you select product for association.'));
          } elseif ($row['price'] < 1) {
            form_set_error('rows][' . $id . '][price', t('Price should be positive number.'));
          } elseif ($row['price'] > $row['commerce_price_amount']['value']) {
            form_set_error('rows][' . $id . '][price', t('Price should be smaller then default price.'));
          }
          
          if (empty($row['stock'])) {
            form_set_error('rows][' . $id . '][stock', t('Stock is required when you select product for association.'));
          } elseif ($row['stock'] < 1) {
            form_set_error('rows][' . $id . '][stock', t('Stock should be positive number.'));
          } elseif ($row['stock'] > 99999) {
            form_set_error('rows][' . $id . '][stock', t('Stock limit not be greater than 99999.'));
          }

          if (empty($row['stock_low_limit_notification'])) {
            form_set_error('rows][' . $id . '][stock_low_limit_notification', t('Stock low limit is required for Notification.'));
          } elseif ($row['stock_low_limit_notification'] < 1) {
            form_set_error('rows][' . $id . '][stock_low_limit_notification', t('Stock should be positive number.'));
          } elseif ($row['stock_low_limit_notification'] > $row['stock']) {
            form_set_error('rows][' . $id . '][stock_low_limit_notification', t('Stock limit should not be greater than Current Stock.'));
          }
        }
      }
      break;
    case 'cancel' :
      $_SESSION[ASSO_EDIT_SESS] = null;
      unset($_SESSION[ASSO_EDIT_SESS]);

      $queryString = array();
      if (isset($_GET['page']))
        $queryString['page'] = $_GET['page'];
      if (!empty($params)) {
        $queryString = array_merge($queryString, $params);
      }
      drupal_goto($data['back']);
      exit;
      break;
    case 'un-archive' :
      if (isset($data['rows']) && array_sum($data['rows']) > 0) {

        $tmp = array();
        foreach ($data['rows'] as $key => $val) {
          if ($val > 0)
            $tmp[$val] = $val;
        }

        $query = db_update('eck_association');
        $query->fields(array('archive' => 0, 'stock' => 0, 'changed' => time()));
        $query->condition('product', $tmp, 'IN');
        $query->condition('store', $data['store_id']);
        $resultSet = $query->execute();
        drupal_set_message(t("Association un-archived successfully"));
        drupal_goto($data['back'], array('query' => $queryString));
      }
      else {
        form_set_error('global', t('Please select atleast one association'));
      }
      break;
  }
}

/**
 * Submit handler for association forms.
 * @global type $user
 * @param type $form
 * @param type $form_state
 */
function findit_merchant_inventory_list_form_submit($form, &$form_state) {
  global $user;
  $association = $form_state['values']['rows'];
  $store_id = $form_state['values']['store_id'];
  $unique_store_id = $form_state['values']['unique_store_id'];
  $dump = json_decode($form_state['values']['dump']);

  try {
    foreach ($association as $key => $value) {
      $data = $dump->{$key};
      $entity = null;
      if (isset($data->id) && $data->id > 0) {
        $entity = eckentity_load($data->id, 'association');
      } else {
        $entity = entity_create('association', array('type' => 'association'));
      }

      $wrapper = entity_metadata_wrapper('association', $entity);

      $wrapper->title->set('Association_product_' . $key . '_store_' . $store_id);
      $wrapper->status->set($value['status']);
      $wrapper->price->set($value['price']); // As commerce stores price in cents.
      $wrapper->product->set($key);
      $wrapper->stock_low_limit_notification->set($value['stock_low_limit_notification']);
      $wrapper->stock->set($value['stock']);

      $wrapper->returnable->set($value['note']['returnable']);
      $wrapper->gift_wrap->set($value['note']['gift_wrap']);

      //unset returnable and gift_wrap
      unset($value['note']['returnable']);
      unset($value['note']['gift_wrap']);

      foreach ($value['note'] as $k => $v) {
        if ($v == 1) {
          $value['note'][$k] = $_SESSION['seller_additional_notes']['note_' . $k];
        } else {
          $value['note'][$k] = '';
        }
      }

      $wrapper->note->set(implode('||', $value['note']));

      $wrapper->store->set($store_id);
      $wrapper->unique_store_id->set($unique_store_id);
      $wrapper->sku->set($data->sku);
      $wrapper->tid->set($data->tid);
      $wrapper->sync_stage->set(1);
      $wrapper->changed->set(time());
      $wrapper->save();
    }

    drupal_set_message(t('Successfully association saved'));
    unset($_SESSION[ASSO_EDIT_SESS]);

    $queryString = array();
    if (isset($_GET['page']))
      $queryString['page'] = $_GET['page'];
    if (isset($_GET['term']))
      $queryString['term'] = $_GET['term'];
    drupal_goto($form_state['values']['back'], array('query' => $queryString));
    exit;
  } catch (Exception $e) {
    drupal_set_message(t('Error in  association saving please try again !!!'), 'error');
  }
}

/**
 * Callback function to get association header data.
 * @global type $user
 * @param type $store
 * @param type $type
 * @return string
 */
function getassociationheader($store, $type) {
  global $user;
  $store_id = $store->store_id;

  $associations = array();
  $innerQuery = db_select('eck_association', 'ea');
  $innerQuery->fields('ea');
  $innerQuery->condition('ea.uid', $user->uid);
  $innerQuery->execute();

  $query = db_select('commerce_product', 'cp');
  $query->addJoin('RIGHT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
  $query->fields('ea');
  $query->fields('cp', array('title'));

  if (isset($_GET['term']) && $_GET['term'] != '') {
    $searchTerm = explode('|', base64_decode($_GET['term']));
    $tsku = reset($searchTerm);
    $or = db_or();
    $or->condition('cp.sku', '%' . trim($tsku) . '%', 'LIKE');
    $or->condition('cp.title', '%' . trim($tsku) . '%', 'LIKE');
    $query->condition($or);
  }

  $query->isNotNull('ea.id');
  $resultSet = $query->execute()->fetchAll();

  $data = array('all' => 0, 'depleting' => 0, 'archive' => 0);
  $data['all'] = count($resultSet);
  foreach ($resultSet as $key => $val) {
    $data['depleting'] += ($val->stock <= 1 || $val->stock <= $val->stock_low_limit_notification) ? 1 : 0;
    $data['archive'] += $val->archive == 1 ? 1 : 0;
  }

  //delete archive in total
  $data['all'] = $data['all'] - $data['archive'];

  $data['total'] = array_sum($data);
  $class = array('all' => $type == 'all' ? 'active' : '',
    'deplete' => $type == 'deplete' ? 'active' : '',
    'archive' => $type == 'archive' ? 'active' : '',
    'request' => $type == 'request' ? 'active' : '');
  $queryString = array();
  //print_r($class); die;
  if (isset($_GET['term']) && $_GET['term'] != '')
    $queryString['term'] = $_GET['term'];

  $current_apath = current_path();
  $output = '<div class="archive-wrapper">';
  if (!empty($queryString['term'])) {
    $global_result['term'] = base64_decode($queryString['term']);
    $global_result['count'] = $data['total'];
    $_SESSION['asso_global_search_result'][$store_id] = $global_result;
  } else {
    unset($_SESSION['asso_global_search_result'][$store_id]);
  }

  $output .= "<ul class=tab-list>";
  $output .= "<li>" . l(sprintf('All (%d)', $data['all']), sprintf('/merchant/%d/inventory/all', $store_id), array('query' => $queryString, 'attributes' => array('class' => array(sprintf('%s', $class['all']))))) . "</li>";
  $output .= "<li>" . l(sprintf('Stock Alert (%d)', $data['depleting']), sprintf('/merchant/%d/inventory/deplete', $store_id), array('query' => $queryString, 'attributes' => array('class' => array(sprintf('%s', $class['deplete']))))) . "</li>";
  $output .= "<li>" . l(sprintf('Archive (%d)', $data['archive']), sprintf('/merchant/%d/inventory/archive', $store_id), array('query' => $queryString, 'attributes' => array('class' => array(sprintf('%s', $class['archive']))))) . "</li>";
  $output .= "<li>" . l(sprintf('Requested Products'), sprintf('/merchant/%d/sku_list', $store_id), array('attributes' => array('class' => array(sprintf('%s', $class['request']))))) . "</li>";
  $output .= "</ul>";
  $output .= "</div></div>";
  return $output;
}

/**
 * Callback function to get default association header.
 * @param type $type
 * @return array
 */
function association_header_new_merchant($type = 'edit') {
  switch ($type) {
    case 'add' :
      $return = array('sku' => array('data' => t('Item Details'), 'field' => 'cp.title',),
        'commerce_price_amount' => array('data' => t('MRP (RM)'), 'field' => 'fdcp.commerce_price_amount',),
        'size' => array('data' => t('Size')),
        'color' => array('data' => t('Color')),
      );
      break;
    case 'edit' :
      $return = array('sku' => array('data' => t('Item Details'), 'field' => 'cp.title', 'width' => '26%'),
        'status' => array('data' => t('Active'), 'field' => 'ea.status',),
        'stock_low_limit_notification' => array('data' => t('Limit'), 'field' => 'ea.stock_low_limit_notification',),
        'stock' => array('data' => t('Current'), 'field' => 'ea.stock',),
        'commerce_price_amount' => array('data' => t('MRP'), 'field' => 'fdcp.commerce_price_amount'),
        'price' => array('data' => t('Seller'), 'field' => 'ea.price',),
        'note' => array('data' => t('Seller Note'), 'width' => '20%'),
      );
      break;
    case 'viewlist' :
      $return = array('sku' => array('data' => t('Item Details')),
        'commerce_price_amount' => array('data' => t('MRP (RM)')),
        'size' => array('data' => t('Size')),
        'color' => array('data' => t('Color')),
        'action' => array('data' => '')
      );
      break;
    case 'processcart2' :
      $return = array('sku' => array('data' => t('Item Details'), 'style' => array('width:28%;')),
        'status' => array('data' => t('Active')),
        'stock_low_limit_notification' => array('data' => t('Limit'),),
        'stock' => array('data' => t('Current'),),
        'commerce_price_amount' => array('data' => t('MRP'),),
        'price' => array('data' => t('Seller'),),
        'note' => array('data' => t('Seller Note')),
        'action' => array('data' => 'Action')
      );
      break;
  }
  return $return;
}

####################################################################################################
# Add Inventory section
####################################################################################################

function findit_merchant_inventory_add($store) {
  drupal_set_title('Inventory');
  $output = sprintf('
    <div class="back-wrapper inventory-back">
        <span class="back">
            %s
        </span>
    </div>
    <div class="inventory-form-wrapper steps-wrapper">
    <div class="steps-wrapper-header">
        Step 1 : Add item to Associate
        <div class="pull-right">Step 1 of 2</div>
    </div>
    <div class="steps-wrapper-content">', l('Add Inventory', 'merchant/' . $store->store_id . '/inventory/all'));

  if (!isset($_REQUEST['table'])) {
    $assoNewSearchForm = drupal_get_form('findit_merchant_association_search_new_form');
    $output .= drupal_render($assoNewSearchForm);
  }
  $output .= '</div></div>';
  $output .= l('View List', 'merchant/' . $store->store_id . '/inventory/viewlist/nojs', array('attributes' => array('class' => 'ctools-use-modal ctools-modal-viewlist-modal hide', 'id' => 'viewlistmain')));
  $downloadLink = l('Download List', 'merchant/association/service', array('query' => array('opr' => 'downloadcart')));

  if (isset($_SESSION[ASSO_SESS]) && count($_SESSION[ASSO_SESS]) > 0) {
    $output .= sprintf('<p><div class="flash-error"></div></p>
                            <p><div class="flash-success">
                            <div class="alert alert-info alert-dismissable">
                                <button aria-hidden="true" data-dismiss="alert" class="close" type="button">×</button>
                                <div class="text-center">*%d Items added click Next to proceed - <a href="javascript:void(0)" id="viewlistbtn">View List</a> / %s</div>
                            </div></div></p>', count($_SESSION[ASSO_SESS]), $downloadLink);
  } else {
    $output .= '<p><div class="flash-error"></div></p>
                <p><div class="flash-success"></div></p>';
  }

  $assoListNewForm = drupal_get_form('findit_merchant_association_list_new', $store);
  $output .= drupal_render($assoListNewForm);
  return $output;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @param type $store
 * @return string
 */
function findit_merchant_association_search_new_form($form, &$form_state, $store = FALSE) {
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#attributes' => array('placeholder' => 'Search for SKU id,Item Title', 'class' => array('search')),
    '#autocomplete_path' => 'merchant/allassociation/autocomplete/sku',
    '#default_value' => isset($_GET['title']) ? base64_decode($_GET['title']) : '',
    '#description' => t('Search by product title.'),
  );

  $form['mode'] = array(
    '#type' => 'radios',
    '#title' => t('Mode'),
    '#description' => t('Select your mode of association'),
    '#options' => array(
      'new' => t('Non-Associated Items'),
      'all' => t('All Items')),
    '#default_value' => isset($_GET['mode']) ? $_GET['mode'] : 'new',
  );

  $category = variable_get('findit_product_association_vocabulary', 'product_category');

  if (!isset($category)) {
    if (user_access('administer product association')) {
      drupal_set_message(t('Default product association category is not set, set it !here.', array('!here' => l(t('here'), 'admin/commerce/config/product-association', array('query' => array('destination' => current_path()))))), 'warning');
    }
    $form['description']['#markup'] = '<p>' . t('No categories are available to associate.') . '</p>';
    return $form;
  }

  $vid = db_query('SELECT vid FROM {taxonomy_vocabulary} WHERE machine_name = :machine_name', array(':machine_name' => $category))->fetchField();

  $form['category'] = array(
    '#type' => 'hierarchical_select',
    '#title' => t('Select Category'),
    '#size' => 1,
    '#default_value' => isset($_GET['category']) ? $_GET['category'] : '',
    '#config' => array(
      'module' => 'hs_taxonomy',
      'params' => array(
        'vid' => $vid,
        'editability' => array(
          'max_levels' => 3,
        ),
      ),
      'resizable' => 0,
      'animation_delay' => 400,
      'render_flat_select' => 0,
      'level_labels' => array(
        'status' => 1,
        'labels' => array(
          0 => t('Select a category'),
          1 => t('Select a sub-category'),
          2 => t('Select a sub-category'),
          3 => t('Select a sub-category'),
          4 => t('Select a sub-category'),
        ),
      ),
    )
  );

  $form['current_path'] = array(
    '#type' => 'hidden',
    '#value' => current_path(),
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="text-center">',
    '#suffix' => '</div>',
    '#value' => t('Go'),
  );
  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 */
function findit_merchant_association_search_new_form_submit($form, &$form_state) {

  $values = $form_state['values'];
  $data = array();
  if ($values['title'] != '')
    $data['title'] = base64_encode(trim($values['title']));
  if ($values['category'][0] != '')
    $data['category'] = $values['category'][0];
  if ($values['mode'] != '')
    $data['mode'] = trim($values['mode']);

  $data = array_filter($data);
  $form_state['redirect'] = array($values['current_path'], array('query' => $data));
}

/**
 *
 * @global type $base_path
 * @global type $user
 * @param type $form
 * @param type $form_state
 * @param type $store
 * @return string|boolean
 */
function findit_merchant_association_list_new($form, &$form_state, $store) {
  global $base_path;
  global $user;

  $data = count($_GET) > 1 ? $_GET : false;
  if ($data) {

    //get all associtaion of current merchant
    $query = db_select('eck_association', 'ea');
    $query->fields('ea', array('product'));
    $query->condition('ea.uid', $user->uid);
    $tmp = $query->execute()->fetchAll();

    $associatedProduct = array();
    foreach ($tmp as $key => $val) {
      $associatedProduct[$val->product] = $val->product;
    }

    $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
    $query->leftjoin('commerce_product_type', 'cpt', 'cpt.type = cp.type');
    $query->leftjoin('field_data_commerce_price', 'fdcp', 'fdcp.entity_id = cp.product_id');
    $query->leftjoin('field_data_catalog_findit', 'fdcf', 'fdcf.entity_id = cp.product_id');
    $query->leftjoin('taxonomy_term_data', 'ttd', 'ttd.tid = fdcf.catalog_findit_tid');
    $query->fields('cp', array('product_id', 'title', 'sku'));
    $query->fields('cpt', array('name'));
    $query->fields('ttd', array('name'));
    $query->fields('fdcp', array('commerce_price_amount', 'commerce_price_currency_code'));

    //conditions
    if (isset($_GET['title']) && $_GET['title'] != '') {
      $tsku = reset(explode('|', base64_decode($_GET['title'])));
      $or = db_or();
      $or->condition('cp.sku', '%' . trim($tsku) . '%', 'LIKE');
      $or->condition('cp.title', '%' . trim($tsku) . '%', 'LIKE');
      $query->condition($or);
    }

    if (isset($_GET['category']) && $_GET['category'] != '') {
      $term = taxonomy_term_load($_GET['category']);
      $term_childrens = array_merge(array($term->tid), array_keys(taxonomy_get_children_all($term->tid, $term->vid)));
      $query->condition('ttd.tid', $term_childrens, 'IN');
    }

    $_GET['mode'] = isset($_GET['mode']) ? $_GET['mode'] : 'all';
    switch ($_GET['mode']) {
      case 'new' :
        if (count($associatedProduct) > 0) {
          $query->condition('cp.product_id', $associatedProduct, 'NOT IN');
        }
        break;
    }

    $header = association_header_new_merchant('add');
    $query->orderByHeader($header);
    $query->limit(10);
    $tmp = $query->execute()->fetchAll();

    $rows = array();
    foreach ($tmp as $key => $val) {
      $row = array();

      $sku_detail_link = '';
      if (isset($val->id) && $val->id > 0) {
        $sku_detail_link = sprintf(SKU_DETAIL_URL, $val->product_id, $val->sku);
      } else {
        $sku_detail_link = 'product/' . $val->sku;
      }

      $isAssociated = in_array($val->product_id, $associatedProduct);

      $row['sku'] = sprintf('<div class="sku">%s</div>
                                    <div class="category">%s (%s)</div>
                                    <div class="title">%s</div>
                                    <div class="tag %s">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $isAssociated ? 'bag-turquoise' : 'bag-violet', $isAssociated ? 'Associated' : 'Non-Associated');

      $variation = _findit_merchant_get_product_variation_by_product_id($val->product_id);
      $variation['size'] = isset($variation['size']) && $variation['size'] != '' ? $variation['size'] : '--';
      $variation['color'] = isset($variation['color']) && $variation['color'] != '' ? $variation['color'] : '--';

      $row['commerce_price_amount'] = sprintf('<p class="green text-center">%s</p>', number_format($val->commerce_price_amount / 100, 2));
      $row['size'] = array('data' => $variation['size'], 'class' => 'text-center');
      $row['color'] = array('data' => $variation['color'], 'class' => 'text-center');
      $rows[$val->product_id] = $row;
    }

    $nxt_stp_clss = 'inactive';
    $addtolist_cls= 'disabled';
    if (isset($_SESSION[ASSO_SESS]) && count($_SESSION[ASSO_SESS]) > 0) {
      $nxt_stp_clss = 'active';
      $addtolist_cls='enabled';
    }
//    $form['tableheader'] = array('#type' => 'markup',
//      '#markup' => sprintf('<h3 class="pull-left notopmargin">Select Items</h3>
//                                     %s
//                                     <button id="btn-addtolist" type="button" class="btn btn-success pull-right">+ Add to list</button>', l('Next Step', 'merchant/' . $store->store_id . '/inventory/processcart', array('attributes' => array('class' => array('btn btn-blue pull-right', $nxt_stp_clss))))),
//    );
//
//    drupal_add_js(sprintf('var base_path = "%s"', $base_path), 'inline');

    $form['tableheader'] = array('#type' => 'markup',
      '#markup' => sprintf('<h3 class="pull-left notopmargin">Select Items</h3>
                                     
                                     <button id="btn-nextstep" type="button" class="btn btn-success nextstep pull-right %s" > Next Step</button>
                                     <button id="btn-addtolist" type="button" class="btn btn-success addtolist pull-right %s">+ Add to list</button>', $nxt_stp_clss,$addtolist_cls),
    );
    drupal_add_js(sprintf('var base_path = "%s"', $base_path), 'inline');
    drupal_add_js(sprintf('var store_id = "%d"', $store->store_id), 'inline'); 
    drupal_add_js(sprintf('var processcart_totalitem = "%d"', isset($_SESSION[ASSO_SESS]) ? count($_SESSION[ASSO_SESS]) : 0), 'inline');
    $form['table'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $rows,
      '#default_value' => 0,
      '#attributes' => array(
        'class' => array(
          'tableheader-processed'),
      ),
      '#empty' => 'No matching results are found.'
    );

    $form['pager'] = array('#type' => 'markup',
      '#markup' => theme('pager'),
    );

    return $form;
  }
  return false;
}

/**
 *
 * @global type $user
 * @global type $base_path
 * @param type $form
 * @param type $form_state
 * @param type $store
 * @return string
 */
function findit_merchant_inventory_processcart($form, &$form_state, $store) {

  global $user;
  global $base_path;

  if (count($_SESSION[ASSO_SESS]) < 1) {
    drupal_set_message('Please choose product first', 'error');
    drupal_goto('merchant/' . $store->store_id . '/inventory/add');
    exit;
  }

  $innerQuery = db_select('eck_association', 'ea');
  $innerQuery->fields('ea');
  $innerQuery->condition('ea.uid', $user->uid);
  $innerQuery->execute();

  $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
  $query->addJoin('LEFT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
  $query->leftjoin('commerce_product_type', 'cpt', 'cpt.type = cp.type');
  $query->leftjoin('field_data_commerce_price', 'fdcp', 'fdcp.entity_id = cp.product_id');
  $query->leftjoin('field_data_catalog_findit', 'fdcf', 'fdcf.entity_id = cp.product_id');
  $query->leftjoin('taxonomy_term_data', 'ttd', 'ttd.tid = fdcf.catalog_findit_tid');
  $query->leftjoin('commerce_store', 'cs', 'cs.unique_store_id = ea.unique_store_id');
  $query->fields('cp', array('product_id', 'title', 'sku'));
  $query->fields('cs', array('title'));
  $query->fields('cpt', array('name'));
  $query->fields('ttd', array('tid', 'name'));
  $query->fields('fdcp', array('commerce_price_amount', 'commerce_price_currency_code'));
  $query->fields('ea');

  $query->condition('cp.product_id', $_SESSION[ASSO_SESS], 'IN');

  $header = association_header_new_merchant('processcart2');
  $query->orderByHeader($header);
  $query->limit(50);
  $tmp = $query->execute()->fetchAll();
  ;

  $rows = $dump = array();
  $rows['#tree'] = TRUE;

  foreach ($tmp as $key => $val) {

    $dump[$val->product_id] = $val;
    $row = array('#attributes' => '');

    $row = array();
    $sku_detail_link = '';
    if (isset($val->id) && $val->id > 0) {
      $sku_detail_link = sprintf(SKU_DETAIL_URL, $val->product_id, $val->sku);
    } else {
      $sku_detail_link = 'product/' . $val->sku;
    }

    $row['sku'] = array(
      '#type' => 'markup',
      '#markup' => sprintf('<div class="sku">%s</div>
                                    <div class="category">%s (%s)</div>
                                    <div class="title">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank')))),
    );


    $row['status'] = array('#type' => 'select',
      '#options' => array(1 => t('Yes'), 0 => t('No')),
      '#default_value' => 1,
      '#required' => true,
      '#attributes' => array('class' => array('text-center'))
    );

    $row['stock_low_limit_notification'] = array(
      '#type' => 'textfield',
      '#element_validate' => array('element_validate_integer'),
      '#default_value' => $val->stock_low_limit_notification ? $val->stock_low_limit_notification : 0,
      '#size' => 11,
      '#attributes' => array('min' => 1, 'max' => 99, 'class' => array('text-center'))
    );

    $row['stock'] = array('#type' => 'textfield',
      '#element_validate' => array('element_validate_integer'),
      '#default_value' => $val->stock ? $val->stock : 0,
      '#size' => 11,
      '#attributes' => array('min' => 1, 'max' => 99999, 'class' => array('text-center'))
    );

    $row['commerce_price_amount']['markup'] = array(
      '#type' => 'markup',
      '#markup' => floatval(number_format($val->commerce_price_amount / 100, 2, '.', '')),
      '#attributes' => array('class' => array('text-center green'))
    );
    $row['commerce_price_amount']['value'] = array(
      '#type' => 'hidden',
      '#value' => floatval(number_format($val->commerce_price_amount / 100, 2, '.', ''))
    );

    $row['commerce_price_amount']['sku'] = array(
      '#type' => 'hidden',
      '#value' => $val->sku
    );

    $row['price'] = array('#type' => 'textfield',
      '#element_validate' => array('element_validate_integer'),
      '#default_value' => $val->price ? intval(number_format($val->price, 2, '.', '')) : intval(number_format($val->commerce_price_amount / 100, 2, '.', '')),
      '#size' => 11,
      '#attributes' => array('min' => 1, 'max' => intval(number_format($val->commerce_price_amount / 100, 2, '.', '')), 'class' => array('text-center'))
    );

    $row['note'] = array('#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE);

    foreach ($_SESSION['seller_additional_notes'] as $k => $v) {
      $row['note'][$k] = array('#type' => 'checkbox', '#title' => $v,);
    }

    $row['note']['returnable'] = array(
      '#type' => 'checkbox',
      '#default_value' => $val->returnable ? $val->returnable : 0,
      '#title' => 'Return Option',
    );

    $row['note']['gift_wrap'] = array(
      '#type' => 'checkbox',
      '#default_value' => $val->gift_wrap ? $val->gift_wrap : 0,
      '#title' => 'Gift Wrap',
    );

    $row['action'] = array(
      '#type' => 'markup',
      '#attributes' => array('class' => array('text-center')),
      '#markup' => l('<i class="icon-SMEDelete"></i>', 'merchant/association/service', array('html' => true,
        'attributes' => array('class' => array('removefromcart-processcart'), 'product' => $val->product_id),
        'query' => array('opr' => 'removetocart', 'product' => $val->product_id))));
    $rows[$val->product_id] = $row;
  }

  $form['products'] = array(
    '#theme' => 'findit_product_association_formtable',
    '#header' => $header,
    '#attributes' => array('id' => 'processcart-table'),
  );

  if (count($rows) > 0) {
    $form['products']['rows'] = $rows;
  } else {
    $form['description']['#markup'] = '<h3>' . t('No match found') . '</h3>';
  }

  $form['products']['store_id'] = array('#type' => 'hidden', '#value' => $store->store_id,);
  $form['products']['unique_store_id'] = array('#type' => 'hidden', '#value' => $store->unique_store_id,);
  $form['products']['dump'] = array('#type' => 'hidden', '#value' => json_encode($dump));

  $form['submit']['#weight'] = -1;
  $form['submit']['#prefix'] = '<div class="pull-right archive-wrapper">';
  $form['submit']['#suffix'] = '</div>';

  $form['submit'][] = array(array('#type' => 'markup', '#markup' => l('Add More', 'merchant/' . $store->store_id . '/inventory/add', array('attributes' => array('class' => array('btn-blue btn'))))),
    array('#attributes' => array('class' => array('btn btn-success')),
      '#type' => 'submit',
      '#value' => 'Done',
      '#ajax' => array(
        'callback' => 'findit_merchant_inventory_processcart_ajaxcallback',
        'wrapper' => 'processcart-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
      )
    )
  );

  $form['#prefix'] = sprintf('<div id="processcart-wrapper">
    <div class="inventory-form-upperwrapper backarrow">%s</div>
    <div class="inventory-form-wrapper steps-wrapper">
    <div class="steps-wrapper-header">
        Step 2 : Complete Association
        <div class="pull-right">Step 2 of 2</div>
    </div></div>', l('Add Inventory', 'merchant/' . $store->store_id . '/inventory/add'));

  $form['#suffix'] = '</div>';
  drupal_set_title('');
  return $form;
}

/**
 *
 * @global type $variables
 * @param type $form
 * @param type $form_state
 * @return type
 */
function findit_merchant_inventory_processcart_ajaxcallback($form, &$form_state) {
  global $variables;
  if ($form_state['executed'] == 1) {
    $store_id = $form_state['values']['store_id'];
    ctools_include('ajax');
    ctools_include('modal');

    $html = sprintf('<div class="icon-Confirm text-center confirmation-message"></div>
                        <div class="modal_message text-center">
                            <h3>%s</h3>
                        </div>', theme_status_messages($variables));

    $commands = array();
    $commands[] = ctools_ajax_command_redirect('merchant/' . $store_id . '/inventory/all', 3 * 1000);
    $commands[] = ctools_modal_command_display('', $html);
    print ajax_render($commands);
    exit;
  }
  return $form;
}

/**
 *
 * @param type $form
 * @param type $form_state
 * @return type
 */
function findit_merchant_inventory_processcart_validate($form, &$form_state) {
  if (isset($form_state['values']['rows']) && !empty($form_state['values']['rows'])) {
    foreach ($form_state['values']['rows'] as $id => $row) {
      $tmpSKU = $row['commerce_price_amount']['sku'];
      if (empty($row['price']) || ($row['price'] < 1)) {
        form_set_error('rows][' . $id . '][price', t('@sku Price is required when you select product for association', array('@sku' => $tmpSKU)));
      } elseif ($row['price'] > $row['commerce_price_amount']['value']) {
        form_set_error('rows][' . $id . '][price', t('@sku Price should be smaller then default price', array('@sku' => $tmpSKU)));
      }

      if (empty($row['stock']) || ($row['stock'] < 1)) {
        form_set_error('rows][' . $id . '][stock', $row['sku'] . t('@sku Stock is required when you select product for association', array('@sku' => $tmpSKU)));
      } elseif ($row['stock'] > 99999) {
        form_set_error('rows][' . $id . '][stock', t('@sku Stock limit should not be greater than 99999', array('@sku' => $tmpSKU)));
      }

      if (empty($row['stock_low_limit_notification']) || ($row['stock_low_limit_notification'] < 1)) {
        form_set_error('rows][' . $id . '][stock_low_limit_notification', t('@sku Stock low limit is required for Notification', array('@sku' => $tmpSKU)));
      } elseif ($row['stock_low_limit_notification'] > $row['stock']) {
        form_set_error('rows][' . $id . '][stock_low_limit_notification', t('@sku Stock low limit should not be greater than Stock', array('@sku' => $tmpSKU)));
      }
    }
  }
  return $form;
}

/**
 *
 * @global type $user
 * @param type $form
 * @param type $form_state
 */
function findit_merchant_inventory_processcart_submit($form, &$form_state) {
  global $user;
  $association = $form_state['values']['rows'];
  $store_id = $form_state['values']['store_id'];
  $unique_store_id = $form_state['values']['unique_store_id'];
  $dump = json_decode($form_state['values']['dump']);

  try {
    foreach ($association as $key => $value) {
      $data = $dump->{$key};
      $entity = null;
      if (isset($data->id) && $data->id > 0) {
        $entity = eckentity_load($data->id, 'association');
      } else {
        $entity = entity_create('association', array('type' => 'association'));
      }

      $wrapper = entity_metadata_wrapper('association', $entity);
      $wrapper->title->set('Association_product_' . $key . '_store_' . $store_id);
      $wrapper->status->set($value['status']);
      $wrapper->price->set($value['price']); // As commerce stores price in cents.
      $wrapper->product->set($key);
      $wrapper->stock_low_limit_notification->set($value['stock_low_limit_notification']);
      $wrapper->stock->set($value['stock']);

      $wrapper->returnable->set($value['note']['returnable']);
      $wrapper->gift_wrap->set($value['note']['gift_wrap']);

      //unset returnable and gift_wrap
      unset($value['note']['returnable']);
      unset($value['note']['gift_wrap']);

      foreach ($value['note'] as $k => $v) {
        if ($v == 1) {
          $value['note'][$k] = $_SESSION['seller_additional_notes'][$k];
        } else {
          $value['note'][$k] = '';
        }
      }

      $wrapper->note->set(implode('||', $value['note']));

      $wrapper->store->set($store_id);
      $wrapper->unique_store_id->set($unique_store_id);
      $wrapper->sku->set($data->sku);
      $wrapper->tid->set($data->tid);
      $wrapper->statusbyadmin->set(1);
      $wrapper->sync_stage->set(1);
      $wrapper->changed->set(time());
      $wrapper->save();
    }

    drupal_set_message(t('Items Successfully added to inventory list'));
    unset($_SESSION[ASSO_SESS]);
  } catch (Exception $e) {
    drupal_set_message(t('Error in  association saving please try again !!!'), 'error');
  }
  $form_state['redirect'] = array('merchant/' . $store_id . '/inventory/all', array('query' => array('result' => 'done')));
}

/**
 *
 * @param type $store
 * @param type $ajax
 * @return string
 */
function findit_merchant_association_viewlist($store, $ajax) {

  $output = '';
  if (isset($_SESSION[ASSO_SESS]) && count($_SESSION[ASSO_SESS]) > 0) {
    $header = association_header_new_merchant('viewlist');

    $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
    $query->leftjoin('commerce_product_type', 'cpt', 'cpt.type = cp.type');
    $query->leftjoin('field_data_commerce_price', 'fdcp', 'fdcp.entity_id = cp.product_id');
    $query->leftjoin('field_data_catalog_findit', 'fdcf', 'fdcf.entity_id = cp.product_id');
    $query->leftjoin('taxonomy_term_data', 'ttd', 'ttd.tid = fdcf.catalog_findit_tid');
    $query->fields('cp', array('product_id', 'title', 'sku'));
    $query->fields('cpt', array('name'));
    $query->fields('ttd', array('tid', 'name'));
    $query->fields('fdcp', array('commerce_price_amount', 'commerce_price_currency_code'));

    $query->condition('cp.product_id', $_SESSION[ASSO_SESS], 'IN');

    $query->limit(10);
    $data = $query->execute()->fetchAll();

    $rows = array();
    foreach ($data as $key => $val) {
      $row = array();
      $sku_detail_link = 'product/' . $val->sku;
      $row['sku'] = sprintf('<div class="sku">%s</div>
                                    <div class="category">%s (%s)</div>
                                    <div class="title">%s</div>', l($val->sku, $sku_detail_link, array('attributes' => array('target' => '_blank'))), $val->name, $val->ttd_name, l($val->title, $sku_detail_link, array('attributes' => array('target' => '_blank'))));

      $variation = _findit_merchant_get_product_variation_by_product_id($val->product_id);
      $variation['size'] = isset($variation['size']) && $variation['size'] != '' ? $variation['size'] : '--';
      $variation['color'] = isset($variation['color']) && $variation['color'] != '' ? $variation['color'] : '--';

      $row['commerce_price_amount'] = array('data' => sprintf('<p class="green">%s</p>', number_format($val->commerce_price_amount / 100, 2)), 'class' => 'text-center');
      $row['size'] = array('data' => $variation['size'], 'class' => 'text-center');
      $row['color'] = array('data' => $variation['color'], 'class' => 'text-center');
      $row['action'] = array('data' => l('<i class="icon-SMEDelete"></i>', 'merchant/association/service', array('html' => true,
          'attributes' => array('class' => array('removefromcart-viewlist'), 'product' => $val->product_id),
          'query' => array('opr' => 'removetocart', 'product' => $val->product_id))), 'class' => 'text-center');
      $rows[$val->product_id] = array('data' => $row, 'id' => 'row-' . $val->product_id);
    }

    $attributes = array('id' => 'viewlist-table');
    $table = array('header' => $header, 'rows' => $rows, 'attributes' => $attributes, 'sticky' => FALSE);
    ;
    $output .= sprintf('<div class="sales_stock_wrapper">
                    <div class="pull-left"><h3 style="margin: 0;text-transform: uppercase;">Select Items</h3></div>
                    <div class="pull-right">
                       %s %s
                    </div>
                   </div>', l('Download List', 'merchant/association/service', array('query' => array('opr' => 'downloadcart'), 'attributes' => array('class' => array('btn-blue')))), l('Next Step', 'merchant/' . $store->store_id . '/inventory/processcart', array('attributes' => array('class' => array('btn-blue')))));
    $output .= theme_table($table);
    $output .= theme('pager');
  } else {
    $output = 'Empty List';
  }

  if ($ajax) {
    ctools_include('modal');
    ctools_include('ajax');

    $command = array();
    $command[] = ctools_modal_command_display('Items to Add in Inventory ', $output);
    print ajax_render($command);
    exit;
  } else {
    return $output;
  }
}

####################################################################################################
# Add New function to download Inventory and upload bulk upload associations
####################################################################################################

function findit_association_bulk_upload($form, &$form_state, $store) {
  global $user;
  //$form['#action'] = url('merchant/'.$store->store_id.'/inventory/bulkupload');
  $form['#attributes']['enctype'] = "multipart/form-data";
  $form['file'] = array(
    '#type' => 'file',
    '#attributes' => array('class' => array('search hide')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#prefix' => '<div class="text-center">',
    '#suffix' => '</div>',
    '#value' => t('Bulk Upload'),
  );
  $form_state['store'] = $store;
  return $form;
}

/**
 * Validate bulk upload of association.
 * @global type $user
 * @param type $form
 * @param type $form_state
 */
function findit_association_bulk_upload_validate($form, &$form_state) {
  global $user;
  $validators = array('file_validate_extensions' => array('csv'));
  $file = file_save_upload('file', $validators);
  if (!$file) {
    form_set_error('file', t('Upload valid file.'));
  } else {
    $handle = fopen('/tmp/' . $file->filename, 'r');
    if ($handle) {
      //Read header of csv
      $row = fgetcsv($handle);
      //Read row by row of csv
      $skusFromCSV = array();
      while ($row = fgetcsv($handle, 0, "\t")) {
        if (!empty($row[0]) && (stripos($row[0], 'sku') === FALSE) && (substr($row[0], 0, 1) != ',')) {
          if (!is_array($row[0]) && (strpos($row[0], ',') !== FALSE)) {
            $row_data = explode(',', $row[0]);
          } else {
            $row_data = $row;
          }
          $skusFromCSV[$row_data[0]] = $row_data;
        }
      }

      //get sku information from commerce product tables
      $innerQuery = db_select('eck_association', 'ea');
      $innerQuery->fields('ea');
      $innerQuery->condition('ea.uid', $user->uid);
      $innerQuery->execute();

      $query = db_select('commerce_product', 'cp');
      $query->addJoin('LEFT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
      $query->leftjoin('commerce_product_type', 'cpt', 'cpt.type = cp.type');
      $query->leftjoin('field_data_commerce_price', 'fdcp', 'fdcp.entity_id = cp.product_id');
      $query->leftjoin('field_data_catalog_findit', 'fdcf', 'fdcf.entity_id = cp.product_id');
      $query->leftjoin('taxonomy_term_data', 'ttd', 'ttd.tid = fdcf.catalog_findit_tid');
      $query->fields('cp', array('product_id', 'title', 'sku'));
      $query->fields('cpt', array('name'));
      $query->fields('ttd', array('name', 'tid'));
      $query->fields('fdcp', array('commerce_price_amount', 'commerce_price_currency_code'));
      $query->fields('ea');

      $query->condition('cp.sku', array_keys($skusFromCSV), 'IN');

      $tmp = $query->execute();

      $skusFromDBS = array();
      while ($row1 = $tmp->fetchAssoc()) {
        $skusFromDBS[$row1['sku']] = $row1;
      }

      $isError = false;
      foreach ($skusFromCSV as $key => $val) {
        if (!array_key_exists($key, $skusFromDBS)) {
          $isError = true;
          form_set_error('global', t($key . ' not a valid SKU.'));
        }

        //check stock always greater then 0 and less then 99999
        if ($val[3] <= 0 || $val[3] > 99999) {
          $isError = true;
          form_set_error('global', t($key . ' Stock always greater then 0 and less then 99999.'));
        }

        //check stock notification always greater then 0 and less then seller current seller stock
        if ($val[2] <= 0 || $val[2] > $val[3]) {
          $isError = true;
          form_set_error('global', t($key . ' Stock notification always greater then 0 and less then seller current stock.'));
        }

        //check seller price always smaller then default price
        if ($val[4] > $skusFromDBS[$key]['commerce_price_amount']) {
          $isError = true;
          form_set_error('global', t($key . ' Seller Price should be smaller then default price.'));
        }
      }

      if ($isError) {
        form_set_error('global', t('CSV file has errors please upload valid file and try again...'));
      } else {
        foreach ($skusFromCSV as $key => $val) {
          if (isset($skusFromDBS[$key]['id']) && $skusFromDBS[$key]['id'] != '') {
            $form_state['associations'][] = array_merge($val, array('id' => $skusFromDBS[$key]['id'],
              'tid' => $skusFromDBS[$key]['tid'],
              'sku' => $skusFromDBS[$key]['sku'],
              'product_id' => $skusFromDBS[$key]['product_id']));
          } else {
            $form_state['associations'][] = array_merge($val, array('tid' => $skusFromDBS[$key]['tid'],
              'sku' => $skusFromDBS[$key]['sku'],
              'product_id' => $skusFromDBS[$key]['product_id']));
            ;
          }
        }
      }
    } else {
      form_set_error('file', t('Unable to access file or file is missing.'));
    }
  }
}

/**
 * Submit handler for bulk upload of association
 * @param type $form
 * @param type $form_state
 */
function findit_association_bulk_upload_submit($form, &$form_state) {

  try {
    $associations = array();
    //get user store
    $store = $form_state['store'];
    foreach ($form_state['associations'] as $key => $val) {
      $extra_data[$val['sku']] = array(
        'id' => $val['id'],
        'tid' => $val['tid'],
        'sku' => $val['sku'],
        'product_id' => $val['product_id'],
      );
    }

    $validators = array('file_validate_extensions' => array('csv'));
    $file = file_save_upload('file', $validators);

    $file->filepath = drupal_realpath($file->uri);
    $file->status = FILE_STATUS_PERMANENT;

    file_save($file);

    if (!$file) {
      form_set_error('file', t('Upload valid file.'));
    } else {

      findit_association_bulk_upload_build_batch($file->filepath, $extra_data, $store);
//      $handle = fopen($file->filepath, 'r'); 
//      if($handle){
//        //Read header of csv
//        $row = fgetcsv($handle);
//        
//        while($row = fgetcsv($handle,0,"\t")){
//          if (!is_array($row[0]) && (strpos($row[0], ',') !== FALSE)) {
//            $row_data = explode(',', $row[0]);
//          } else {
//            $row_data = $row;
//          }
//          $row_data = array_merge($row_data, $extra_data[$row_data[0]]);
//          $associations[$row_data[0]] = $row_data;
//        }
//      }
    }

    // Callback function to set batch data and further process.
    // findit_association_bulk_upload_build_batch($extra_data, $store);
  } catch (Exception $e) {
    drupal_set_message(t('Error in  association saving please try again !!!'), 'error');
  }
}

/**
 * Callback function to download the association import file.
 */
function _findit_association_download_file() {
  $data = '';
  $params = drupal_get_query_parameters();
  $filetype = isset($params['filetype']) ? $params['filetype'] : '';
  $store_id = isset($params['store_id']) ? $params['store_id'] : '';
  if (isset($_SESSION['association_data_' . $store_id])) {
    $data = $_SESSION['association_data_' . $store_id];
    unset($_SESSION['association_data_' . $store_id]);
  }
  ob_clean();
  switch ($filetype) {
    case 'csv' :
      $filename = "association_" . date('Y_m_d_h_i_s') . ".csv";
      drupal_add_http_header('Content-Type', 'text/csv; utf-8');
      drupal_add_http_header('Content-Disposition', 'attachment; filename=' . $filename);
      drupal_add_http_header('Cache-Control', 'max-age=300; must-revalidate');
      print $data;
      break;
    case 'excel' :
      $filename = "association_" . date('Y_m_d_h_i_s') . ".xls";
      drupal_add_http_header('Content-Type: application/vnd.ms-excel; utf-8');
      drupal_add_http_header('Content-Disposition', 'attachment; filename=' . $filename);
      drupal_add_http_header('Cache-Control', 'max-age=300; must-revalidate');
      print $data;
      break;
  }
  drupal_exit();
}

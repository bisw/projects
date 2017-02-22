<?php
function findit_merchant_store_list(){
    global $user;
    
     //Define table header
    $header = array();
    $header['title'] = array('data' => t('Title'),'field' => 'title');
    $header['status'] = array('data' => t('Status'),'field' => 'status');
    $header['opr'] = t('Operations');
    
    //Create Query
    $query = db_select('commerce_store', 'cs')->extend('PagerDefault');
    $query->fields('cs',array('store_id','title','status'));
    if(isset($_GET['title']) && $_GET['title']!=''){
        $query->condition('cs.title','%'.$_GET['title'].'%','like');
    }
    $query->condition('cs.uid',$user->uid);
    $query->orderByHeader($header);
    $query->limit(10);
    $stores = $query->execute()->fetchAll();
    
    $rows = array();
    $true = theme('image',array('path'=>drupal_get_path('module','findit_association').'/webroot/images/true.png'));
    $false = theme('image',array('path'=>drupal_get_path('module','findit_association').'/webroot/images/false.png'));  
    foreach($stores as $key=>$val){
        $rows[$val->store_id] = array('title'=>$val->title,
                                      'status'=>$val->status ? $true : $false,
                                      'opr'=>l('Edit','store/'.$val->store_id.'/edit',array('query'=>drupal_get_destination())).
                                               '&nbsp;|&nbsp;'.l('View Associations','merchant/'.$val->store_id.'/associations'));
    }
    
    $output = '';
    if (!empty($rows)) {
        $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'sort-table')));
        $output .= theme('pager');
    }else {
        $output .= t("No results found.");
    }
    return $output;
}

function findit_merchant_association_search($form, &$form_state,$store){
    $form['title'] = array(
        '#type' => 'textfield',
        '#title' => t('Title'),
        '#default_value'=> isset($_GET['title']) ? $_GET['title'] : '',
        '#description' => t('Search by product title.'),
    );
    
    $form['sku'] = array(
        '#type' => 'textfield',
        '#title' => t('SKU'),
        '#default_value'=> isset($_GET['sku']) ? $_GET['sku'] : '',
        '#description' => t('Search by product sku.'),
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
        '#default_value' =>  isset($_GET['category']) ? $_GET['category'] : '',
        '#config' => array(
            'module' => 'hs_taxonomy',
            'params' => array(
            'vid' => $vid,
        ),
        
        'save_lineage'    => 0,
        'enforce_deepest' => 0,
        'entity_count'    => 0,
        'require_entity'  => 0,
        'resizable'       => 0,
        'level_labels' => array(
        'status' => 0,
        'labels' => array(
            0 => t('Main category'),
            1 => t('Subcategory'),
            2 => t('Third level category'),
            ),
        ),
        'dropbox' => array(
        'status'   => 0,
        'title'    => t('All selections'),
        'limit'    => 0,
        'reset_hs' => 1,
        ),
        'editability' => array(
            'status'           => 0,
            'item_types'       => array(),
            'allowed_levels'   => array(
            0 => 0,
            1 => 0,
            2 => 1,
            ),
            'allow_new_levels' => 0,
            'max_levels'       => 3,
        ),
        // These settings cannot be configured through the UI: they can only be
        // overridden through code.
        'animation_delay'    => 400,
        'special_items'      => array(),
        'render_flat_select' => 0,
        )
    );
    
    $form['mode'] = array(
        '#type' => 'radios',
        '#title' => t('Mode'),
        '#description' => t('Select your mode of association'),
        '#options' => array(
            'all' => t('All'),
            'my' => t('Only Associated'),
            'new' => t('Not Associated')),
        '#default_value' => isset($_GET['mode']) ? $_GET['mode'] : 'my',
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
        '#value' => t('Search'),
    );
    
    $tmp = isset($_SESSION[ASSO_SESS]) ? count($_SESSION[ASSO_SESS]) : 0 ;
    $form['actions']['cartprocess'] = array(
        '#type' => 'markup',
        '#markup' => l(sprintf('Association Bucket (%d)',$tmp),sprintf('merchant/%d/association/processcart',$store->store_id),array('attributes'=>array('class'=>'button radius','id'=>'processcart_but'))).'<div class="loading"></div>',
    );
    
    return $form;
}

function findit_merchant_association_search_submit($form, &$form_state){
    
    $values = $form_state['values'];
    $data = array();
    if($values['title'] != '') $data['title'] = trim($values['title']);
    if($values['sku'] != '') $data['sku'] = trim($values['sku']);
    if($values['category'][0] != '') $data['category'] = $values['category'][0];
    if($values['mode'] != '') $data['mode'] = trim($values['mode']);
    
    $data = array_filter($data);
    $form_state['redirect'] = array($values['current_path'],array('query' => $data));
}

function findit_merchant_association_list($store){
    
    global $user;
    global $base_path;
    global $pager_total_items;
    
    $header = association_header_merchant();
    
    $associations = array();
    $innerQuery = db_select('eck_association','ea');
    $innerQuery->fields('ea');
    $innerQuery->condition('ea.uid',$user->uid);
    $innerQuery->execute();
    
    $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
    $query->addJoin('LEFT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
    $query->leftjoin('commerce_product_type','cpt','cpt.type = cp.type');
    $query->leftjoin('field_data_commerce_price','fdcp','fdcp.entity_id = cp.product_id');
    $query->leftjoin('field_data_catalog_findit','fdcf','fdcf.entity_id = cp.product_id');
    $query->leftjoin('taxonomy_term_data','ttd','ttd.tid = fdcf.catalog_findit_tid');
    $query->fields('cp',array('product_id','title','sku'));
    $query->fields('cpt',array('name'));
    $query->fields('ttd',array('name'));
    $query->fields('fdcp',array('commerce_price_amount','commerce_price_currency_code'));
    $query->fields('ea');
    if(isset($_GET['title']) && $_GET['title'] != ''){
        $query->condition('cp.title','%'.$_GET['title'].'%','like');
    }
    if(isset($_GET['sku']) && $_GET['sku'] != ''){
        $query->condition('cp.sku',trim($_GET['sku']));
    }
    if(isset($_GET['category']) && $_GET['category'] != ''){
        $term = taxonomy_term_load($_GET['category']);
        $term_childrens = array_merge(array($term->tid),array_keys(taxonomy_get_children_all($term->tid, $term->vid)));
        $query->condition('ttd.tid',$term_childrens,'IN');                                                     
    }
    
    $_GET['mode'] = isset($_GET['mode']) ? $_GET['mode'] : 'my';
    switch($_GET['mode']){
        case 'my' :
            $query->isNotNull('ea.id');
            break;
        case 'new' :
            $query->isNull('ea.id');
            break;
    }
    
    $query->orderByHeader($header);
    $query->limit(10);
    $tmp = $query->execute();
    
    $true = theme('image',array('path'=>drupal_get_path('module','findit_association').'/webroot/images/true.png'));
    $false = theme('image',array('path'=>drupal_get_path('module','findit_association').'/webroot/images/false.png'));  
    while($row = $tmp->fetchAssoc()){
        
        $associations[$row['product_id']] = array();
        if($store->status == 1 && ($row['statusbyadmin'] == 1 || $row['id'] == '')){
            if(array_key_exists($row['product_id'],$_SESSION[ASSO_SESS])){
                $associations[$row['product_id']]['product_id'] = sprintf('<input type="button" class="form-submit addtocart" value="Remove to Bucket" opr="removetocart" product="%d">',$row['product_id'],$row['product_id']);
            }else{
                $associations[$row['product_id']]['product_id'] = sprintf('<input type="button" class="form-submit addtocart" value="Add to Bucket" opr="addtocart" product="%d">',$row['product_id'],$row['product_id']);
            }
        }elseif($row['statusbyadmin'] == 0){
            $associations[$row['product_id']]['product_id'] = 'SKU Deactivated';
        }else{
            $associations[$row['product_id']]['product_id'] = 'Store Deactivated';
        }
        
        $sku_detail_link = '';
        if(isset($row['id']) && $row['id'] > 0){
            $sku_detail_link = sprintf(SKU_DETAIL_URL,$row['product_id'],$row['sku']);
        }else{
            $sku_detail_link = 'product/'.$row['sku'];
        }
        
        $associations[$row['product_id']]['title'] = l($row['title'],$sku_detail_link,array('attributes'=>array('target'=>'_blank')));
        $associations[$row['product_id']]['sku'] = $row['sku'];
        $associations[$row['product_id']]['name'] = $row['name'];
        $associations[$row['product_id']]['ttd_name'] = $row['ttd_name'];
        $associations[$row['product_id']]['commerce_price_currency_code'] = $row['commerce_price_currency_code'];
        $associations[$row['product_id']]['commerce_price_amount'] = number_format($row['commerce_price_amount'] / 100,2);

        $associations[$row['product_id']]['price'] = '--';
        $associations[$row['product_id']]['stock_low_limit_notification'] = '--';
        $associations[$row['product_id']]['stock'] = '--';
        $associations[$row['product_id']]['status'] = '--';
        $associations[$row['product_id']]['statusbyadmin'] = '--';
        $associations[$row['product_id']]['returnable'] = '--';
        $associations[$row['product_id']]['gift_wrap'] = '--';
        $associations[$row['product_id']]['note'] = '--';
        $associations[$row['product_id']]['notebymerchant'] = '--';
        $associations[$row['product_id']]['notebyadmin'] = '--';
        
        if($row['id'] > 0){
            $associations[$row['product_id']]['price'] = number_format($row['price'],2);
            $associations[$row['product_id']]['stock_low_limit_notification'] = $row['stock_low_limit_notification'];
            $associations[$row['product_id']]['stock'] = $row['stock'];
            $associations[$row['product_id']]['status'] = $row['status'] == 1 ? $true : $false;
            $associations[$row['product_id']]['statusbyadmin'] = $row['statusbyadmin'] == 1 ? $true : $false;
            $associations[$row['product_id']]['returnable'] = $row['returnable'] == 1 ? $true : $false;
            $associations[$row['product_id']]['gift_wrap'] = $row['gift_wrap'] == 1 ? $true : $false;
            
            $tmp_note = explode('||',$row['note']);
            $tmp_note_str = '<ul>';
            foreach($tmp_note as $k=>$v){
                $tmp_note_str .= sprintf('<li>%s</li>',$v);
            }
            $tmp_note_str .= '</ul>';
            $associations[$row['product_id']]['note'] = $tmp_note_str;
            $associations[$row['product_id']]['notebymerchant'] = $row['notebymerchant'];
            $associations[$row['product_id']]['notebyadmin'] = $row['notebyadmin'];
        }
    }
    
    $variables = array(
    'header' => $header,
    'rows' => $associations,
    'attributes' => array(),
    'colgroups' => array(),
    'sticky' => TRUE,
    'empty' => t('You do not have any products associated.'),
    );
    
    drupal_add_js(sprintf('var base_path = "%s"',$base_path),'inline');
    drupal_add_js(sprintf('var processcart_totalitem = "%d"',isset($_SESSION[ASSO_SESS]) ? count($_SESSION[ASSO_SESS]) : 0),'inline');
    $output = drupal_render(drupal_get_form('findit_merchant_association_search',$store));
    $output .= 'Total records : '. number_format(reset($pager_total_items));
    $output .= theme_table($variables);
    $output .= theme('pager');
    return $output;
}

function findit_merchant_association_service(){
    global $base_path;
    $queryString = $_GET;
    $return = array('result'=>false);
    if(isset($_GET['opr'])){
        switch($_GET['opr']){
            case 'addtocart' :
                $products = isset($_GET['product']) ? explode(',',$_GET['product']) : false;
                if(count($products) > 0){
                    foreach($products as $key=>$val){
                        $_SESSION[ASSO_SESS][$val] = $val;
                    }
                    $return = array('result'=>true,
                                    'message'=>sprintf('*%d Items added click Next to proceed - <a href="javascript:void(0)" id="viewlistbtn">View List</a> / %s',count($_SESSION[ASSO_SESS]),
                                        l('Download List', 'merchant/association/service', array('query' => array('opr'=>'downloadcart')))));
                }
                break;
            case 'removetocart' :
                if(isset($_GET['product'])){
                  $productLists = array();
                  $productLists = explode(',', $_GET['product']);
                  foreach ($productLists as $productId) {
                    if(array_key_exists($productId,$_SESSION[ASSO_SESS])){
                        unset($_SESSION[ASSO_SESS][$productId]);
                        $return = array('result'=>true);
                    }
                  }
                }
                break;
            case 'downloadcart' :
                $tmp = array();
                  if(isset($_SESSION[ASSO_SESS])) {
                    foreach ($_SESSION[ASSO_SESS] as $key => $val) {
                      if ($val > 0) $tmp[$key] = $key;
                    }
                  }
                  $op = 'downloadcart';
                  findit_merchant_download_association_file($tmp, 'csv', $op);
            break; 
        }
    }
    return drupal_json_output($return);
}

function findit_merchant_association_autocomplete($term) {
  global $user;

  $return = array();
  if (strlen($term) < 4) {
    return drupal_json_output($return);
  }
  switch (strtolower(arg(3))) {
    case 'sku' :
      $innerQuery = db_select('eck_association', 'ea');
      $innerQuery->fields('ea');
      $innerQuery->condition('ea.uid', $user->uid);
      $innerQuery->execute();

      $or = db_or();
      $or->condition('cp.title', '%' . $term . '%', 'like');
      $or->condition('cp.sku', '%' . $term . '%', 'like');

      $query = db_select('commerce_product', 'cp');
      $query->addJoin('RIGHT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
      $query->fields('cp', array('sku', 'title'));
      $query->condition($or);
      $query->range(0, 10);

      $query->isNotNull('ea.id');
      $resultSet = $query->execute()->fetchAll();

      foreach ($resultSet as $key => $val) {
        $tmp = sprintf('%s | %s', $val->sku, $val->title);
        $return[$val->sku] = $tmp;
      }
      break;
  }
  return drupal_json_output($return);
}

function findit_merchant_allassociation_autocomplete($term, $type){
    
    $return = array();
    if (strlen($term) < 4) {
      return drupal_json_output($return);
    }
    switch(strtolower(arg(3))){
        case 'sku' :
            $or = db_or();
            $or->condition('cp.title','%'.$term.'%','like');
            $or->condition('cp.sku','%'.$term.'%','like');
            
            $query = db_select('commerce_product','cp');
            $query->fields('cp',array('sku','title'));
            $query->condition($or);
            $query->range(0,10);
            $data = $query->execute()->fetchAll();
            
            foreach($data as $key=>$val){
                $tmp = sprintf('%s | %s',$val->sku,$val->title);
                $return[$val->sku] = $tmp;
            }
            break;
    }
    return drupal_json_output($return);
}

function findit_merchant_association_processcart($form, &$form_state,$store){
    global $user;
    $associations = array();
    if(isset($_SESSION[ASSO_SESS]) && count($_SESSION[ASSO_SESS]) > 0){
        $associations = array();
        $innerQuery = db_select('eck_association','ea');
        $innerQuery->fields('ea');
        $innerQuery->condition('ea.uid',$user->uid);
        $innerQuery->execute();
        
        $query = db_select('commerce_product', 'cp')->extend('TableSort')->extend('PagerDefault');
        $query->addJoin('LEFT OUTER', $innerQuery, 'ea', 'ea.product = cp.product_id');
        $query->leftjoin('commerce_product_type','cpt','cpt.type = cp.type');
        $query->leftjoin('field_data_commerce_price','fdcp','fdcp.entity_id = cp.product_id');
        $query->leftjoin('field_data_catalog_findit','fdcf','fdcf.entity_id = cp.product_id');
        $query->leftjoin('taxonomy_term_data','ttd','ttd.tid = fdcf.catalog_findit_tid');
        $query->leftjoin('commerce_store','cs','cs.unique_store_id = ea.unique_store_id');
        $query->fields('cp',array('product_id','title','sku'));
        $query->fields('cs',array('title'));
        $query->fields('cpt',array('name'));
        $query->fields('ttd',array('tid','name'));
        $query->fields('fdcp',array('commerce_price_amount','commerce_price_currency_code'));
        $query->fields('ea');
        
        $query->condition('cp.product_id',$_SESSION[ASSO_SESS],'IN');
        
        $query->limit(10);
        $tmp = $query->execute();
        
        $dump = array();
        $associations['#tree'] = TRUE;
        while($row = $tmp->fetchAssoc()){
            
            $dump[$row['product_id']] = $row;
            $associations[$row['product_id']]['product_id'] = array(
                '#type' => 'markup',
                '#markup' => $row['product_id'],
            );
            
            $sku_detail_link = '';
            if(isset($row['id']) && $row['id'] > 0){
                $sku_detail_link = sprintf(SKU_DETAIL_URL,$row['product_id'],$row['sku']);
            }else{
                $sku_detail_link = 'product/'.$row['sku'];
            }
            
            $associations[$row['product_id']]['title'] = array(
                '#type' => 'markup',
                '#markup' => l($row['title'],$sku_detail_link,array('attributes'=>array('target'=>'_blank'))),
            );
            
            $associations[$row['product_id']]['sku'] = array(
                '#type' => 'markup',
                '#markup' => $row['sku'],
            );
            
            $associations[$row['product_id']]['name'] = array(
                '#type' => 'markup',
                '#markup' => $row['name'],
            );
            
            $associations[$row['product_id']]['ttd_name'] = array(
                '#type' => 'markup',
                '#markup' => $row['ttd_name'],
            );
            
            $associations[$row['product_id']]['commerce_price_currency_code'] = array(
                '#type' => 'markup',
                '#markup' => $row['commerce_price_currency_code'],
            );
            
            $associations[$row['product_id']]['commerce_price_amount'] = array(
                '#type' => 'textfield',
                '#value' => number_format($row['commerce_price_amount'] / 100,2),
                '#size' => 11,
                '#attributes' => array('disabled'=>'disabled'),
            );
            
            $associations[$row['product_id']]['price'] = array(
                '#title' => t('price'),
                '#title_display' => 'invisible',
                '#field_prefix' => 'RM',
                '#type' => 'textfield',
                '#element_validate' => array('element_validate_integer'),
                '#default_value' => intval(number_format($row['commerce_price_amount'] / 100,2)),
                '#size' => 11,
                '#maxlength' => 11,
                '#attributes' => array('min'=>1,
                                       'max'=>number_format($row['commerce_price_amount'] / 100,2))
            );
            
            $associations[$row['product_id']]['stock_low_limit_notification'] = array(
                '#type' => 'textfield',
                '#title' => $row['product_id'],
                '#title_display' => 'invisible',
                '#size' => 3,
                '#maxlength' => 3,
                '#default_value' => $row['stock_low_limit_notification'],
                '#attributes' => array('default_value'=>$row['stock_low_limit_notification']),
                '#attributes' => array('min'=>1,'max'=>100)
            );
            
            $associations[$row['product_id']]['stock'] = array(
                '#title' => t('Stock'),
                '#title_display' => 'invisible',
                '#type' => 'textfield',
                '#element_validate' => array('element_validate_integer'),
                '#default_value' => intval($row['stock']),
                '#size' => 5,
                '#maxlength' => 5,
                '#attributes' => array('min'=>1,'max'=>99999)
            );
            
            $associations[$row['product_id']]['status'] = array(
                '#type' => 'checkbox',
                '#title' => $row['product_id'],
                '#title_display' => 'invisible',
                '#default_value' => $row['status'],
                '#attributes' => array('default_value'=>$row['status'])
            );
            
            $associations[$row['product_id']]['returnable'] = array(
                '#type' => 'checkbox',
                '#title' => $row['product_id'],
                '#title_display' => 'invisible',
                '#default_value' => $row['returnable'],
            );
            
            $associations[$row['product_id']]['gift_wrap'] = array(
                '#type' => 'checkbox',
                '#title' => $row['product_id'],
                '#title_display' => 'invisible',
                '#default_value' => $row['gift_wrap'],
            );
            
            $row['note'] = explode('||',$row['note']);
            
            $associations[$row['product_id']]['note'][0] = array(
                '#type' => 'textfield',
                '#title' => t('Additional Note'),
                '#title_display' => 'invisible',
                '#default_value' => $row['note'][0],
            );
            
            $associations[$row['product_id']]['note'][1] = array(
                '#type' => 'textfield',
                '#title' => t('Additional Note'),
                '#title_display' => 'invisible',
                '#default_value' => $row['note'][1],
            );
            
            $associations[$row['product_id']]['note'][2] = array(
                '#type' => 'textfield',
                '#title' => t('Additional Note'),
                '#title_display' => 'invisible',
                '#default_value' => $row['note'][2],
            );
            
            $associations[$row['product_id']]['notebymerchant'] = array(
                '#type' => 'textarea',
                '#title' => t('Additional Note'),
                '#title_display' => 'invisible',
                '#default_value' => $row['notebymerchant'],
            );
        }
    }
    
    $form['products'] = array(
      '#theme' => 'findit_product_association_formtable',
      '#header' => association_header_merchant('processcart'),
    );
    
    $form['products']['store_id'] =  array('#type' => 'hidden','#value' => $store->store_id,);
    $form['products']['unique_store_id'] =  array('#type' => 'hidden','#value' => $store->unique_store_id,);
    $form['products']['dump'] =  array('#type' => 'hidden','#value' => json_encode($dump));
    
    $form['pager'] = array(
        '#theme' => 'pager',
    );
    
    $form['actions'] = array(
        '#type' => 'actions',
        '#weight' => -10,
    );
    
    if(count($associations) > 0){
        $form['products']['rows'] = $associations;
        $form['actions']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save'),
            '#attributes' => array('class' => array('sticky-enabled', 'button', 'radius')),
        );
    }else{
        $form['description']['#markup'] = '';
        $form['description']['#markup'] .= '<h3>' . t('No match found') . '</h3>';
    }
    
    $form['actions']['cancel'] = array(
    '#markup' => l(t('Back'),'merchant/'.$store->store_id.'/associations',
                   array('attributes'=>array('class'=>'button radius')))
    );
    
    return $form;
}

function findit_merchant_association_processcart_validate($form, &$form_state){
    if (isset($form_state['values']['rows']) && !empty($form_state['values']['rows'])) {
        foreach ($form_state['values']['rows'] as $id => $row){
            
            if($row['status'] == 0 && $row['notebymerchant'] == ''){
                form_set_error('rows][' . $id . '][notebymerchant', t('SKU disable reason should be required!!!'));
            }
            
            if (empty($row['price'])) {
                form_set_error('rows][' . $id . '][price', t('Price is required when you select product for association'));
            }elseif($row['price'] > ($row['commerce_price_amount'] / 100)){
                form_set_error('rows][' . $id . '][price', t('Price should be smaller then default price'));
            }
            
            if (empty($row['stock'])) {
                form_set_error('rows][' . $id . '][stock', t('Stock is required when you select product for association'));
            }elseif($row['stock'] > 99999){
                form_set_error('rows][' . $id . '][stock', t('Stock limit should not be greater than 99999'));
            }
            
            if (empty($row['stock_low_limit_notification'])) {
                form_set_error('rows][' . $id . '][stock_low_limit_notification', t('Stock low limit is required for Notification'));
            }elseif($row['stock_low_limit_notification'] > $row['stock']){
                form_set_error('rows][' . $id . '][stock_low_limit_notification', t('Stock low limit should not be greater than Stock'));
            }
        }
    }
}

function findit_merchant_association_processcart_submit($form, &$form_state){
    global $user;
    $association = $form_state['values']['rows'];
    $store_id = $form_state['values']['store_id'];
    $unique_store_id = $form_state['values']['unique_store_id'];
    $dump = json_decode($form_state['values']['dump']);
    
    try{
        $disable_sku = array();
        foreach($association as $key=>$value){
            
            $data = $dump->{$key};
            $entity = null;
            if(isset($data->id) && $data->id > 0){
                $entity = eckentity_load($data->id,'association');
            }else{
                $entity = entity_create('association', array('type' => 'association'));
            }
            
            //Get data of disabled sku's
            if($value['status'] == 0){
                $disable_sku[$data->sku] = array('store_id'=>$data->store,
                                                 'store_title'=>$data->cs_title,
                                                 'unique_store_id'=>$data->unique_store_id,
                                                 'sku_title'=>$data->title,
                                                 'note'=>$value['note'],
                                                 'notebymerchant'=>$value['notebymerchant'],
                                                 'sku'=>$data->sku);
            }
            
            $wrapper = entity_metadata_wrapper('association', $entity);
            $wrapper->title->set('Association_product_' . $key . '_store_' . $store_id);
            $wrapper->status->set($value['status']);
            $wrapper->price->set($value['price']); // As commerce stores price in cents.
            $wrapper->product->set($key);
            $wrapper->stock_low_limit_notification->set($value['stock_low_limit_notification']);
            $wrapper->stock->set($value['stock']);
            $wrapper->returnable->set($value['returnable']);
            $wrapper->gift_wrap->set($value['gift_wrap']);
            $wrapper->note->set(implode('||',$value['note']));
            $wrapper->notebymerchant->set($value['notebymerchant']);
            
            $wrapper->store->set($store_id);
            $wrapper->unique_store_id->set($unique_store_id);
            $wrapper->sku->set($data->sku);
            $wrapper->tid->set($data->tid);
            $wrapper->sync_stage->set(1);
            $wrapper->changed->set(time());
            $wrapper->save();
        }
        if(count($disable_sku) > 0){
            findit_association_sendmail('merskuopr',$disable_sku,0);
        }
        
        drupal_set_message(t('Successfully association saved'));
        unset($_SESSION[ASSO_SESS]);
    }catch(Exception $e){
        drupal_set_message(t('Error in  association saving please try again !!!'),'error');
    }
    $form_state['redirect'] = array('merchant/' . $store_id . '/associations');
}

function association_header_merchant($type = 'list'){
    switch($type){
        case 'processcart' :
            $return = array('product_id' => array('data' => t('Product ID'),'field' => 'product_id',),
                t('Title'),
                'sku' => array('data' => t('SKU'),'field' => 'cp.sku',),
                'type' => array('data' => t('Parent Category'),'field' => 'type',),
                'tid' => array('data' => t('Category'),'field' => 'tid',),
                'pcode' => array('data' => t('Price Code'),'field' => 'commerce_price_currency_code',),
                'dprice' => array('data' => t('Default Price'),'field' => 'commerce_price_amount',),
                'price' => array('data' => t('Price'),'field' => 'price',),
                'stock_low_limit_notification' => array('data' => t('Low Limit'),'field' => 'stock_low_limit_notification',),
                'stock' => array('data' => t('Stock'),'field' => 'stock',),
                'status' => array('data' => t('Status'),'field' => 'status',),
                'returnable' => array('data' => t('Returnable'),'field' => 'returnable',),
                'gift_wrap' => array('data' => t('Gift Wrap'),'field' => 'gift_wrap',),
                t('Additional Note'),
                t('Note by Merchant'),
                );
            break;
            default :
            $return = array('product_id' => array('data' => t('Product ID'),'field' => 'product_id',),
                t('Title'),
                'sku' => array('data' => t('SKU'),'field' => 'cp.sku',),
                'type' => array('data' => t('Parent Category'),'field' => 'type',),
                'tid' => array('data' => t('Category'),'field' => 'ttd.tid',),
                'pcode' => array('data' => t('Price Code'),'field' => 'commerce_price_currency_code',),
                'dprice' => array('data' => t('Default Price'),'field' => 'commerce_price_amount',),
                'price' => array('data' => t('Price'),'field' => 'price',),
                'stock_low_limit_notification' => array('data' => t('Stock Low Limit'),'field' => 'stock_low_limit_notification',),
                'stock' => array('data' => t('Stock'),'field' => 'stock',),
                'status' => array('data' => t('Status'),'field' => 'status',),
                'statusbyadmin' => array('data' => t('Status By Admin'),'field' => 'statusbyadmin',),
                'returnable' => array('data' => t('Returnable'),'field' => 'returnable',),
                'gift_wrap' => array('data' => t('Gift Wrap'),'field' => 'gift_wrap',),
                t('Note'),
                t('Additional Note by Merchant'),
                t('Additional Note by Admin'));
            
                break;
    }
    return  $return;
}
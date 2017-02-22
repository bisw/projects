<?php

/**
 * @file
 * Page callbacks.
 */

/**
 * Callback function for review view page.
 */
function findit_reply_view() {
  // theme('fivestar_static', array('widget' => array('name' => 'default1')));
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();

  $form_states = array();
  $form = drupal_get_form('findit_reply_view_form', $form_states);
  $form_state = array();
  $form1 = drupal_get_form('_findit_reply_view', $form_state);
  return drupal_render($form) . drupal_render($form1);
}

/**
 * Callback function for showing disable product review form in popup.
 *
 * @param object $account
 *   The user object.
 * @param string $type
 *   The type of entity.
 *
 * @return renderable form array
 *   The renderable form array.
 */
function findit_reply_view_disable_product_reviews($account, $type) {
  // theme('fivestar_static', array('widget' => array('name' => 'default1')));
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();

  $form_states = array();
  $form = drupal_get_form('findit_reply_view_form', $form_states);
  $form_state = array();
  $form1 = drupal_get_form('_findit_reply_view_disable_product_reviews', $type, $form_state);
  return drupal_render($form) . drupal_render($form1);
}

/**
 * Function for gettig parameters to be sent in query to marketplace.
 */
function _findit_reply_get_parameters($type = FALSE, $op = FALSE) {
  $arg = arg();
  $parameters = array();
  if ($type == 'product') {
    $parameters['entity_type'] = 'commerce_product';
  }
  if ($type == 'store') {
    $parameters['entity_type'] = 'commerce_store';
  }
  if ($type == 'bulk') {
    $parameters['entity_type'] = 'commerce_product';
    $parameters['field_bulk_upload'] = 1;
  }
  if (isset($_GET['type']) && $_GET['type'] != '') {
    $parameters['entity_type'] = 'commerce_' . $_GET['type'];
  }

  if (isset($_GET['status']) && $_GET['status'] !== '') {
    $parameters['status'] = $_GET['status'];
  }
  if (isset($_GET['sku']) && $_GET['sku'] != '') {
    $parameters['sku'] = $_GET['sku'];
  }
  if (isset($_GET['email']) && $_GET['email'] != '') {
    $parameters['email'] = $_GET['email'];
  }
  if (isset($_GET['sort']) && $_GET['sort'] !== '') {
    if ($_GET['sort'] == 'oldest') {
      $sort = 'created';
      $direction = 'ASC';
    }
    else {
      if ($_GET['sort'] == 'created') {
        $sort = $_GET['sort'];
        $direction = 'DESC';
      }
      else {
        $sort = $_GET['sort'];
        $direction = 'ASC';
      }
    }
  }
  elseif ($op == 'disable_product') {
    $sort = 'changed';
    $direction = 'DESC';
  }
  else {
    $sort = 'created';
    $direction = 'DESC';
  }
  if (isset($_GET['id'])) {
    $parameters['id'] = $_GET['id'];
  }
  $parameters['sort'] = $sort;
  $parameters['direction'] = $direction;
  if ($op == 'disable_product') {
    $parameters['deleted'] = 1;
  }
  if ($arg[3] == 'bulk-reviews') {
    $parameters['field_bulk_upload'] = 1;
  }
  else {
    $parameters['field_bulk_upload'] = 0;
  }
  if ($op == 'reported_to_findit') {
    $parameters['reported_to_findit'] = 1;
  }
  if (isset($_GET['deleted']) && $_GET['deleted'] != '') {
    $parameters['deleted'] = 1;
  }
  if ($op == 'reported') {
    $parameters['reported'] = 1;
  }
  return $parameters;
}

/**
 * 
 */
function _findit_reply_view_disable_product_reviews($form, &$form_states, $type) {
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = _findit_reply_get_parameters($type, 'disable_product');
  $sort = $parameters['sort'];
  $direction = $parameters['direction'];
  unset($parameters['sort']);
  unset($parameters['direction']);

  $data = finditreply::index('*', $parameters, $page, 20, $sort, $direction);
 // print_r($data); die;
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];
  $rows = array();
  $attr = array('attributes' => array('class' => 'use-ajax red'));
  $attr_nocolor = array('attributes' => array('class' => 'use-ajax'));
  $attr_tb = array('attributes' => array('target' => '_blank'));
  foreach ($data['data'] as $key => $review) {
    $row = array();
    $link = array();
    if ($review['parent'] != 0) {
      $link[] = l(t('View'), 'review/' . $review['parent'] . '/view', $attr_tb);
    }
    else {
      $link[] = l(t('View'), 'review/' . $review['id'] . '/view', $attr_tb);
    }
    $link[] = l(t('Approve'), 'review/nojs/' . $review['id'] . '/deleted/1', $attr_nocolor);

    $query = db_select('reply_extra_info', 'rei')
        ->fields('rei', array('deleted_date', 'deleted_by'))
        ->condition('rid', $review['id'], '=')
        ->orderBy('deleted_date', 'DESC')
        ->execute();
    $reply_extra_data = $query->fetchAssoc();
    $row['deleted_date'] = custom_dateformat_mFY($reply_extra_data['deleted_date']);
    $user_obj = user_load($reply_extra_data['deleted_by']);
    $row['deleted_by'] = $user_obj->name;
    $row['date'] = custom_dateformat_mFY($review['created']);
    if ($review['uid'] > 1) {
      $surl = variable_get('marketplace_url', 'localhost/findit') . '/findit/' . $review['uid'] . '/product-reviews';
      $row['name'] = l($review['real_name'], $surl, $attr_tb);
      $row['email'] = $review['email'];
    }
    else {
      $row['name'] = l($review['real_name'], $review['parent_url'], $attr_tb);
      $row['email'] = _get_user_email_from_store_id($review['store_id']);
    }
    $subject = $review['parent'] == 0 ? '<b>' . $review['subject'] . '</b><br/>' : '';
    $row['description'] = $subject . substr($review['description'], 0, 200) . '...';
    // $p = $review['parent'] == 0 ? $review['parent_title'] : $review['subject'];
    $p = $review['parent'] == 0 ? '' : $review['subject'];
    // $row['parent'] = l($p, $review['parent_url'], array('attributes' => array('target' => '_blank')));
    $row['parent'] = $p;
    if ($review['type'] == 'product') {
      $row['type'] = $review['parent'] == 0 ? ('product review') : t('comment on product review');
    }
    else {
      $row['type'] = $review['parent'] == 0 ? ('seller review') : t('comment on seller review');
    }
    $row['reference'] = l($review['parent_title'], $review['parent_url'], $attr_tb);
    $row['sku'] = isset($review['sku']) ? $review['sku'] : '';
    $row['rating'] = $review['parent'] == 0 ? round($review['rating'] / 20, 1) : '';
    // $row['status'] = $review['status'] == 0 ? t('Unpublished') : t('Published');
    $row['posted_as_anonymously'] = $review['is_anonymous'] == 1 ? t('Yes') : t('No');

    $row['link'] = implode('<br>', $link);
    $rows[$review['id']] = $row;
  }
  $header = array(
    'date' => t('Date submitted'),
    'name' => t('Submitted by'),
    'email' => t('Customer Email'),
    'description' => t('Review text'),
    'parent' => t('Parent review'),
    // 'description'=>t('Reviews'),
    'type' => t('Type (Review/comment)'),
    'rating' => t('Rating'),
    'reference' => t('Product/Seller Details'),
    'sku' => t('Sku'),
    'link' => t('Actions'),
    'deleted_by' => t('Rejected by'),
    'deleted_date' => t('Rejected date'),
    'posted_as_anonymously' => t('Posted as anonymously'),
  );

  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#attributes' => array('class' => 'fix-width-table'),
    '#empty' => t('No reviews found'),
  );

  $form['pager'] = array(
    '#markup' => theme('pager'),
  );
  return $form;
}

/**
 * @param $form
 * @param $form_states
 * @return mixed
 *
 */
function _findit_reply_view($form, &$form_states) {
  global $pager_page_array, $pager_total, $pager_total_items;
  $arg = arg();
  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = _findit_reply_get_parameters('', '');
  $sort = $parameters['sort'];
  $direction = $parameters['direction'];
  unset($parameters['sort']);
  unset($parameters['direction']);

  $data = finditreply::index('*', $parameters, $page, 20, $sort, $direction);
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];
  $rows = array();
  $attr = array('attributes' => array('class' => 'ctools-use-modal red'));
  $attr_nocolor = array('attributes' => array('class' => 'ctools-use-modal'));
  //$attr_color = array('attributes' => array('class' => 'ctools-use-modal red'));
  $attr_tb = array('attributes' => array('target' => '_blank'));
  foreach ($data['data'] as $review) {
    $row = array();
    $link = array();
    // $link[] = l(t('Views'), 'review/nojs/' . $review['id'], $attr);
    // $link[] = l(t('Edit'), 'review/nojs/' . $review['id'] . '/edit', $attr);

    if ($review['parent'] != 0) {
      $link[] = l(t('View'), 'review/' . $review['parent'] . '/view', $attr_tb);
    }
    else {
      $link[] = l(t('View'), 'review/' . $review['id'] . '/view', $attr_tb);
    }

    $link[] = l(t('Edit'), 'review/nojs/' . $review['id'] . '/edit', $attr_nocolor);

    if ($review['parent'] == 0) {
      //$link[] = l(t('Reply'), 'review/nojs/' . $review['id'] . '/reply', $attr);
    }
    if ($review['status'] == 1) {
      $link[] = l(t('unpublish'), 'review/nojs/' . $review['id'] . '/status/0', $attr);
    }
    else {
      $link[] = l(t('Publish'), 'review/nojs/' . $review['id'] . '/status/1', array('attributes' => array('class' => 'use-ajax')));
    }
    $link[] = l(t('Reject'), 'review/nojs/' . $review['id'] . '/deleted/0', $attr);

    $row['date'] = custom_dateformat_mFY($review['created']);
    if ($review['uid'] > 1) {
      $surl = variable_get('marketplace_url', 'localhost/findit') . '/findit/' . $review['uid'] . '/product-reviews';
      $row['name'] = l($review['real_name'], $surl, $attr_tb);
      $row['email'] = $review['email'];
    }
    else {
      $row['name'] = l($review['real_name'], $review['parent_url'], $attr_tb);
      $row['email'] = _get_user_email_from_store_id($review['store_id']);
    }
    $subject = $review['parent'] == 0 ? '<b>' . $review['subject'] . '</b><br/>' : '';
    $row['description'] = $subject . substr($review['description'], 0, 200) . '...';
    $p = $review['parent'] == 0 ? '' : $review['subject'];
    // $p = $review['parent'] == 0 ? $review['parent_title'] : $review['subject'];
    // $row['parent'] = l($p, $review['parent_url'], array('attributes' => array('target' => '_blank')));
    $row['parent'] = $p;
    if ($review['type'] == 'product') {
      $row['type'] = $review['parent'] == 0 ? ('product review') : t('comment on product review');
    }
    else {
      $row['type'] = $review['parent'] == 0 ? ('seller review') : t('comment on seller review');
    }
    $row['reference'] = l($review['parent_title'], $review['parent_url'], $attr_tb);
    $row['sku'] = isset($review['sku']) ? $review['sku'] : '';
    $row['rating'] = $review['parent'] == 0 ? round($review['rating'] / 20, 1) : '';
    $row['status'] = $review['status'] == 0 ? t('Unpublished') : t('Published');
    if (!isset($arg[3]) || (isset($arg[3]) && ($arg[3] != 'bulk-reviews'))) {
      $row['posted_as_anonymously'] = $review['is_anonymous'] == 1 ? t('Yes') : t('No');
    }
    else {
      $row['system_date'] = strlen($review['system_date']) ? date('d-M-Y H:i', $review['system_date']) : "";
    }

    $row['link'] = implode(' <br> ', $link);
    $row['deleted_by']=$review['approved_by'];
    $row['deleted_date']=(isset($review['approved_date']) && $review['approved_date'] != '')?date('d/m/Y H:i',$review['approved_date']):'';
    $rows[$review['id']] = $row;
  }
  $header = array(
    'date' => t('Date submitted'),
    'name' => t('Submitted by'),
    'email' => t('Customer Email'),
    'description' => t('Review text'),
    'parent' => t('Parent review'),
    // 'description'=>t('Reviews'),
    'type' => t('Type (Review/comment)'),
    'rating' => t('Rating'),
    'reference' => t('Product/Seller Details'),
    'sku' => t('Sku'),
    'link' => t('Actions'),
    'deleted_by' => t('Approved By'),
    'deleted_date' => t('Approved Date'),
  );
  if (!isset($arg[3]) || (isset($arg[3]) && ($arg[3] != 'bulk-reviews'))) {
    $header['posted_as_anonymously'] = t('Posted as anonymously');
  }
  else {
    $header['system_date'] = t('System Date');
  }
  $form['headerf'] = array(
    '#type' => 'fieldset',
    '#title' => t('Action'),
  );
  $form['headerf']['status'] = array(
    '#type' => 'select',
    '#title' => t('Review Status'),
    '#options' => array('' => '', 1 => t('published'), 0 => t('unpublished')),
    '#required' => TRUE,
  );
  $form['headerf']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#attributes' => array('class' => 'fix-width-table'),
    '#empty' => t('No reviews found'),
  );

  $form['pager'] = array(
    '#markup' => theme('pager'),
  );
  return $form;
}

/**
 * Submit function for publish a review.
 */
function _findit_reply_view_submit($form, &$form_states) {
  foreach ($form_states['values']['table'] as $id) {
    $values['action'] = 'publish';
    $values['value'] = $form_states['values']['status'];
    $entity_id = $id;
    finditreply::update($entity_id, $values);
  }
  drupal_set_message(t('Review have been updated.'));
}

/**
 * Function for header filter and sort form of reviews list.
 */
function findit_reply_view_form($form, &$form_states) {
  $arg = arg();
  $form['headerf'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter & Sorting'),
  );
  $form['headerf']['status'] = array(
    '#type' => 'select',
    '#title' => t('Review Status'),
    '#options' => array(
      '' => t('all'),
      1 => t('published'),
      0 => t('unpublished'),
    ),
    '#default_value' => isset($_GET['status']) ? $_GET['status'] : '',
    '#prefix' => '<div class="container-inline">',
  );

  if (($arg[3] != 'bulk-reviews') && ($arg[3] != 'disable-product-reviews') && ($arg[3] != 'disable-seller-reviews') && $arg[2] != 'reported_to_findit_reviews') {
    $form['headerf']['type'] = array(
      '#type' => 'select',
      '#title' => t('Reviews Type'),
      '#options' => array(
        '' => t('all'),
        'product' => t('Product review'),
        'store' => t('Seller review'),
      ),
      '#default_value' => isset($_GET['type']) ? $_GET['type'] : '',
      '#suffix' => '</div>',
    );
  }

  $form['headerf']['sort'] = array(
    '#type' => 'select',
    '#title' => t('Sort By'),
    '#options' => array(
      '' => 'select',
      'created' => 'Latest',
      'oldest' => 'oldest',
      'entity_type' => 'category wise',
      'entity_id' => 'merchant wise',
    ),
    '#default_value' => isset($_GET['sort']) ? $_GET['sort'] : '',
    '#prefix' => '<div class="container-inline">',
  );
  $form['headerf']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Customer email'),
    '#attributes' => array('placeholder' => t('Search by customer email')),
    '#default_value' => isset($_GET['email']) ? $_GET['email'] : '',
  );
  if (arg(3) != 'disable-seller-reviews') {
    $form['headerf']['sku'] = array(
      '#type' => 'textfield',
      '#title' => t('Sku'),
      '#attributes' => array('placeholder' => t('Search by product sku')),
      '#default_value' => isset($_GET['sku']) ? $_GET['sku'] : '',
    );
  }
  $form['headerf']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#suffix' => '</div>',
  );
  $form['#method'] = 'GET';
  return $form;
}

/**
 * Function for sorting form on reviews listing page argument.
 */
function findit_reply_sortdata_form($form, &$form_states) {
  $form['sort'] = array(
    '#type' => 'select',
    '#title' => t('Sort By'),
    '#options' => array(
      // 'all' => t('select'),
      'created' => t('Most Recent'),
      'sort_by_help' => t('Most Helpful'),
    ),
    '#attributes' => array('onChange' => 'document.getElementById("findit-reply-sortdata-form").submit();'),
    '#default_value' => isset($_GET['sort_order']) ? $_GET['sort_order'] : '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply'),
    '#attributes' => array('class' => array('element-invisible')),
  );
  return $form;
}

/**
 * Submit handler for sorting form submit.
 */
function findit_reply_sortdata_form_submit($form, &$form_states) {
  global $base_url;
  $path = current_path();
  $sort_order = $form_states['values']['sort'];
  $url = $base_url . '/' . $path . '?sort_order=' . $sort_order;
  $form_states['redirect'] = $url;
}

/**
 * Admin configuration form for order processing.
 */
function findit_reply_settings($form, $form_state) {
  $form['marketplace_reply_url'] = array(
    '#type' => "textfield",
    '#title' => t("Marketplace Reply service Url"),
    "#description" => t("Marketplace reply REST Url"),
    '#default_value' => variable_get('marketplace_reply_url', ''),
    '#required' => TRUE,
  );
  $form['marketplace_reply_user'] = array(
    '#type' => "textfield",
    '#title' => t("Marketplace Reply service user"),
    "#description" => t("Marketplace reply REST user"),
    '#default_value' => variable_get('marketplace_reply_user', ''),
    '#required' => TRUE,
  );
  $form['marketplace_reply_pass'] = array(
    '#type' => "textfield",
    '#title' => t("Marketplace Reply service user's password"),
    "#description" => t("Marketplace reply REST password"),
    '#default_value' => '',
    '#required' => TRUE,
  );

  return system_settings_form($form);
}

/**
 * Callback function for review actions.
 */
function review_page_callback($js, $id) {
  if ($js) {
    global $user;
    ctools_include('ajax');
    ctools_include('modal');
    $arg = arg();
    if ($arg[3] == 'view') {
      $data = finditreply::index('*', array('id' => $id), 0, 20);
      $reply = $data['data'][0];
      if (!user_access('review findit access') && ($reply['status'] == 0)) {
        $commands[] = ctools_modal_command_display(t('Access denied'), t('Review is either published or you do not have access to view the review.'));
      }
      else {
        ctools_add_css('reply', 'findit_reply', 'css');
        $subject_title = $data['data'][0]['subject'];
        $commands = array();
        $description = reply_post_html($reply);

        $commands[] = ctools_modal_command_display($subject_title, $description);
      }
      print ajax_render($commands);
      exit;
    }
    if ($arg[3] == 'edit') {
      // Edit reviews.
      $data = finditreply::index('*', array('id' => $id), 0, 20);
      $reply = $data['data'][0];
      if (!user_access('review findit access') && ($reply['status'] == 1 || $reply['review_author'] != _get_store_unique_id($user->uid))) {
        $commands[] = ctools_modal_command_display(t('Access denied'), t('Review is either published or you do not have access to edit the review.'));
      }
      else {
        $form_state = array(
          'ajax' => 1,
          'title' => t('Review'),
          'build_info' => array('args' => array($reply)),
        );
        $commands = ctools_modal_form_wrapper('_reply_action_form', $form_state);
        if (!empty($form_state['executed'])) {
          $commands = array();
          $commands[] = ctools_ajax_command_reload();
        }
        updateRatingOnElastic($id);
      }
      print ajax_render($commands);
      exit;
    }
    elseif ($arg[3] == 'delete') {
      // Delete reviews.
      $data = finditreply::index('*', array('id' => $id), 0, 20);
      $reply = $data['data'][0];
      if (!user_access('review findit access') && ($reply['status'] == 1 || $reply['review_author'] != _get_store_unique_id($user->uid))) {
        $commands[] = ctools_modal_command_display(t('Access denied'), t('Review is either published or you do not have access to delete the review.'));
      }
      else {
        $form_state = array(
          'ajax' => 1,
          'title' => t('Review'),
          'build_info' => array('args' => array($reply)),
        );
        $commands = ctools_modal_form_wrapper('_reply_complete_delete_action_form', $form_state);
        if (!empty($form_state['executed'])) {
          $commands = array();
          $commands[] = ctools_ajax_command_reload();
        }
      }
      print ajax_render($commands);
      exit;
    }
    elseif ($arg[3] == 'reply') {
      // Reply on review.
      $form_states = array(
        'ajax' => 1,
        'title' => t('Review'),
        'build_info' => array('args' => array($id)),
      );
      $commands = ctools_modal_form_wrapper('_reply_action_reply_form', $form_states);
      if (!empty($form_state['executed'])) {
        $commands = array();
        $commands[] = ctools_ajax_command_reload();
      }
      print ajax_render($commands);
      updateRatingOnElastic($id);
      exit;
    }
    elseif ($arg[3] == 'mreply') {
      // Reply on review.
      $form_states = array(
        'ajax' => 1,
        'title' => t('Review'),
        'build_info' => array('args' => array($id, $arg[4])),
      );

      $commands = ctools_modal_form_wrapper('_reply_merchant_reply_form', $form_states);
      if (!empty($form_state['executed'])) {
        $commands = array();
        $commands[] = ctools_ajax_command_reload();
      }
      print ajax_render($commands);
      updateRatingOnElastic($id);
      exit;
    }
    if ($arg[3] == 'status') {
      // Published and unpublished.
      $status = $arg[4];
      $data = finditreply::index('*', array('id' => $id), 0, 20);
      $reply = $data['data'][0];
      if ($status == 1) {
        // Published.
        $values['action'] = 'publish';
        $values['value'] = 1;

        $value = finditreply::update($id, $values);
        drupal_set_message(t('Review has been published.'));
        $reply['status'] = 1;
        // findit_reply_send_mail_merchant_comment_status($reply);
        $commands[] = ctools_ajax_command_reload();
        print ajax_render($commands);
        updateRatingOnElastic($id);
        exit;
      }
      else {
        // Unpublished.
        $form_states = array(
          'ajax' => 1,
          'title' => $reply['subject'],
          'build_info' => array('args' => array($reply)),
        );
        $commands = ctools_modal_form_wrapper('_reply_action_unpublish_form', $form_states);
        if (!empty($form_state['executed'])) {
          $commands = array();
          $commands[] = ctools_ajax_command_reload();
        }
        print ajax_render($commands);
        updateRatingOnElastic($id);
        exit;
      }
    }
    else {
      if ($arg[3] == 'deleted') {
        // Published and unpublished.
        $status = $arg[4];
        if ($status == 0) {
          $data = finditreply::index('*', array('id' => $id), 0, 20);
          $reply = $data['data'][0];
          $report_to_findit = $arg[5] == 'report_to_findit' ? TRUE : FALSE;

          // Unpublished.
          $form_states = array(
            'ajax' => 1,
            'title' => $reply['subject'],
            'build_info' => array('args' => array($reply, $report_to_findit)),
          );
          $commands = ctools_modal_form_wrapper('_reply_action_delete_form', $form_states);
          if (!empty($form_state['executed'])) {
            $commands = array();
            $commands[] = ctools_ajax_command_reload();
          }
          print ajax_render($commands);
          updateRatingOnElastic($id);
          exit;
        }
        else {
          $values['action'] = 'deleted';
          $values['value'] = 0;
          $value = finditreply::update($id, $values);
          drupal_set_message(t('Review have been approved.'));
          $commands[] = ctools_ajax_command_reload();
          print ajax_render($commands);
          updateRatingOnElastic($id);
          exit;
        }
      }
      elseif ($arg[3] == 'comment') {
        $store_id = _get_store_unique_id($user->uid);
        $data = finditreply::retrieve($id, '*', 0, $store_id);
        $output = '';
        foreach ($data['data'] as $key => $review) {
          $output .= reply_post_comment_html($review);
        }
        $commands[] = ajax_command_html('.review-reply-body-' . $id, $output);
        print ajax_render($commands);
        exit;
      }
      else {
        $review = finditreply::retrieve($id, '*', 1);
        $commands = array();
        $commands[] = ctools_modal_command_display(t('Reviews') . '' . $id, reset($review));
        print ajax_render($commands);
        exit;
      }
    }
  }
  else {
    drupal_access_denied();
  }
  drupal_goto('<front>');
}

/**
 * Function for updating rating on elastic.
 *
 * @param int $id
 *   Review id.
 */
function updateRatingOnElastic($id) {
  $url = variable_get('intlayerUrl', 'http://localhost/intlayer') . '/updaterating';
  $actdata = array('id' => $id);
  $result = drupal_http_request($url, array(
    'method' => 'POST',
    'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    'data' => http_build_query($actdata),
  ));
  watchdog('updateRating', 'updateRating url hit :url and :data', array(':url' => $url, ':data' => json_encode($result)), WATCHDOG_INFO);
}

/**
 * Form builder function for review delete action.
 */
function _reply_complete_delete_action_form($form, &$form_states, $review = array()) {
  $form['title'] = array(
    '#markup' => '<div class="review-delete-text">' . t('Do you really want to delete this review?') . '</div>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

  return $form;
}

/**
 * Submit handler for review delete form.
 */
function _reply_complete_delete_action_form_submit($form, &$form_states) {
  $reply = $form_states['build_info']['args'][0];
  $value = finditreply::delete($reply['id'], $reply);
  drupal_set_message(t('Review have been Deleted'));
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}

/**
 * Form builder function for comment action.
 */
function _reply_action_form($form, &$form_states, $review = array()) {
  global $user;
  $roles = $user->roles;
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $review['id'],
  );
  if ($review['parent'] == 0) {
    $form['subject'] = array(
      '#title' => t('Title'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => $review['subject'],
    );
  }
  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#required' => TRUE,
    '#default_value' => $review['description'],
  );
  if ($review['parent'] == 0) {
    $form['rating'] = array(
      '#title' => t('Rating'),
      '#type' => 'fivestar',
      '#required' => TRUE,
      '#default_value' => $review['rating'],
      '#chosen' => 0,
    );
  }

  if (empty(in_array("merchant", $roles))) {
    $form['status'] = array(
      '#type' => 'select',
      '#title' => t('Review Status'),
      '#options' => array(1 => t('Publish'), 0 => t('Not publish')),
      '#required' => TRUE,
      '#default_value' => $review['status'],
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for comments actions form.
 */
function _reply_action_form_submit($form, &$form_states) {
  $reply = $form_states['build_info']['args'][0];

  $reply['description'] = $form_states['values']['description'];

  $reply['status'] = $form_states['values']['status'];
  if (isset($form_states['values']['subject'])) {
    $reply['subject'] = $form_states['values']['subject'];
    $reply['rating'] = $form_states['values']['rating'];
  }
  finditreply::update($reply['id'], $reply);
  drupal_set_message(t('Review have been saved.'));
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}

/**
 * Form builder function for reviews action.
 */
function _reply_action_reply_form($form, &$form_states, $parent = 0) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $parent,
  );

  $form['description'] = array(
    '#title' => t('Comments'),
    '#type' => 'textarea',
    '#required' => TRUE,
    '#default_value' => '',
  );

  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('Review Status'),
    '#options' => array(1 => t('Publish'), 0 => t('Not publish')),
    '#required' => TRUE,
    '#default_value' => 1,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Submit handler for reviews actions form.
 */
function _reply_action_reply_form_submit($form, &$form_states) {
  $parent = $form_states['build_info']['args'][0];

  $reply['description'] = $form_states['values']['description'];
  $reply['status'] = $form_states['values']['status'];
  $reply['parent'] = $parent;

  $value = finditreply::create($reply);
  drupal_set_message(t('Review have been saved.'));
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}

/**
 * Form builder function for comment by merchant form.
 */
function _reply_merchant_reply_form($form, &$form_states, $parent = 0, $store_id = NULL) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $parent,
  );
  $form['store_id'] = array(
    '#type' => 'hidden',
    '#value' => $store_id,
  );

  $form['description'] = array(
    '#title' => t('Comments'),
    '#type' => 'textarea',
    '#required' => TRUE,
    '#default_value' => '',
    '#attributes' => array('placeholder' => t('Write a comment')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('REPLY'),
  );
  return $form;
}

/**
 * Submit function for comment by merchant form.
 */
function _reply_merchant_reply_form_submit($form, &$form_states) {
  $reply['description'] = $form_states['values']['description'];
  $reply['parent'] = $form_states['input']['id'];
  $reply['author'] = $form_states['values']['store_id'];
  $reply['by'] = 'merchant';

  $value = finditreply::create($reply);
  drupal_set_message(t('Review have been saved.'));
}

/**
 * Form builder function for unpublish a review.
 */
function _reply_action_unpublish_form($form, &$form_states, $reply = 0) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $reply['id'],
  );

  $form['reason'] = array(
    '#title' => t('Reason'),
    '#type' => 'select_or_other',
    '#required' => TRUE,
    '#options' => array(
      'Abusing language' => t('Abusing language'),
      'Bad review' => t('Bad language'),
    ),
  );

  $form['status'] = array(
    '#type' => 'hidden',
    '#value' => 0,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Unpublish'),
  );

  return $form;
}

/**
 * Submit function for unpublish a review.
 */
function _reply_action_unpublish_form_submit($form, &$form_states) {
  $reply = $form_states['build_info']['args'][0];
  $values['reason'] = $form_states['values']['reason'];
  $values['action'] = 'publish';
  $values['value'] = 0;
  finditreply::update($reply['id'], $values);
  drupal_set_message(t('Review have been saved.'));
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}

/**
 * Form builder function for deleting a review.
 */
function _reply_action_delete_form($form, &$form_states, $reply = 0, $report_to_findit = FALSE) {
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $reply['id'],
  );
  $form['reply_title'] = array(
    '#title' => t('Title'),
    '#type' => 'item',
    '#markup' => $reply['subject'],
  );
  $form['reply_description'] = array(
    '#title' => t('Description'),
    '#type' => 'item',
    '#markup' => $reply['description'],
  );

  $form['reason'] = array(
    '#title' => t('Reason'),
    '#type' => 'select_or_other',
    '#required' => TRUE,
    '#options' => array(
      'Abusing language' => t('Abusing language'),
      'Bad review' => t('Bad language'),
      'Reported' => t('Reported'),
    ),
  );

  $form['status'] = array(
    '#type' => 'hidden',
    '#value' => 0,
  );
  $form['report_to_findit'] = array(
    '#type' => 'hidden',
    '#value' => $report_to_findit,
  );
  $form['reply'] = array(
    '#type' => 'hidden',
    '#value' => $reply,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reject'),
  );
  return $form;
}

/**
 * Submit function for deleting a review.
 */
function _reply_action_delete_form_submit($form, &$form_states) {
  global $user;
  $reply = $form_states['build_info']['args'][0];

  $values['reason'] = $form_states['values']['reason'];
  $values['action'] = 'deleted';
  $values['report_to_findit'] = $form_states['values']['report_to_findit'];
  $values['value'] = 1;
  $review = finditreply::update($reply['id'], $values);
  db_insert('reply_extra_info')
      ->fields(array('rid', 'deleted_date', 'deleted_by'))
      ->values(array(
        'rid' => $reply['id'],
        'deleted_date' => time(),
        'deleted_by' => $user->uid,
      ))
      ->execute();

  // Send mail to merchant for unpunlish of review by findit user.
  if ($form_states['values']['report_to_findit'] == TRUE) {
    // findit_reply_send_mail_merchant_comment_status($form_states['values']['reply']);
    report_to_findit_unpublish_mail_to_merchant($review);
  }
  drupal_set_message(t('Review have been rejected.'));
  $commands[] = ctools_ajax_command_reload();
  print ajax_render($commands);
  exit;
}

/**
 * Function for sending mail to merchant on unpublish raise to findit.
 */
function report_to_findit_unpublish_mail_to_merchant($review) {
  $vars = array();
  $store = findit_reply_order_get_store_unique_id($review['store_id']);
  $vars['account'] = user_load($store->uid);
  $vars['pet_name'] = 'report_to_findit_unpublish_mail_to_merchant';
  $vars['review'] = $review;
  $vars['review']['review_page_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE));
  $vars['review']['review_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE, 'query' => array('id' => $review['id'])));
  findit_reply_send_pet($vars);
}

/**
 * Callback function for reported reviews table.
 */
function findit_reported_reply_view() {
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = _findit_reply_get_parameters('', 'reported');
  $sort = $parameters['sort'];
  $direction = $parameters['direction'];
  unset($parameters['sort']);
  unset($parameters['direction']);

  $data = finditreply::index('*', $parameters, $page, 20, $sort, $direction);
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];
  $rows = array();
  $attr = array('attributes' => array('class' => 'ctools-use-modal'));
  $attr_tb = array('attributes' => array('target' => '_blank'));
  foreach ($data['data'] as $key => $review) {

    $row = array();
    $link = array();
    if ($review['parent'] != 0) {
      $link[] = l(t('View'), 'review/' . $review['parent'] . '/view', $attr_tb);
    }
    else {
      $link[] = l(t('View'), 'review/' . $review['id'] . '/view', $attr_tb);
    }
    $link[] = t('Ignore');
    if ($review['deleted'] == 0) {
      $link[] = l(t('unpublish'), 'review/nojs/' . $review['id'] . '/deleted/0', $attr);
    }
    $row[] = custom_dateformat_mFY($review['created']);
    if ($review['uid'] > 1) {
      $surl = variable_get('marketplace_url', 'localhost/findit') . '/findit/' . $review['uid'] . '/product-reviews';
      $row['name'] = l($review['real_name'], $surl, $attr_tb);
    }
    else {
      $row[] = $review['real_name'];
    }
    $row[] = $review['reported_by'];
    $row[] = custom_dateformat_mFY($review['reported_date']);
    $row[] = $review['email'];
    $subject = $review['parent'] == 0 ? '<b>' . $review['subject'] . '</b><br/>' : '';
    $row[] = $subject . substr($review['description'], 0, 200) . '...';
    // $p = $review['parent'] == 0 ? $review['parent_title'] : $review['subject'];
    $p = $review['parent'] == 0 ? '' : $review['subject'];
    // $row['parent'] = l($p, $review['parent_url'], array('attributes' => array('target' => '_blank')));
    $row[] = $p;
    if ($review['type'] == 'product') {
      $row[] = $review['parent'] == 0 ? ('product review') : t('comment on product review');
    }
    else {
      $row[] = $review['parent'] == 0 ? ('seller review') : t('comment on seller review');
    }
    $row[] = isset($review['sku']) ? $review['sku'] : '';
    $row[] = l($review['parent_title'], $review['parent_url'], $attr_tb);
    $status = $review['status'] == 0 ? t('Unpublished') : t('Published');
    $row[] = $review['deleted'] == 0 ? $status : t('Deleted');
    $row[] = $review['report'];
    $row[] = implode('<br/>', $review['report_reason']);

    // $row[] = $review['deleted'] ==  0 ? $status : t('Deleted');

    $row[] = implode(' | ', $link);
    $rows[] = $row;
  }
  $header = array(
    t('Date submitted'),
    t('Submitted by'),
    t('Reported by'),
    t('Reported Date'),
    t('Customer Email'),
    t('Review Text'),
    t('Parent Review'),
    t('Type (Review/Comment)'),
    t('Sku'),
    t('Product/Seller Details'),
    t('Status'),
    t('No of reported'),
    t('Reason'),
    t('Actions'),
  );

  // Add filter form.
  $form_states = array();
  $form = drupal_get_form('findit_reply_view_form', $form_states);

  return drupal_render($form) . theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No reviews found'),
  )
      ) . theme('pager');
}

/**
 * Callback function for store reviews.
 */
function findit_reply_store_view($commerce_store) {
  // Adding js and css for readmore functionality.
  drupal_add_css(drupal_get_path('module', 'findit_reply') . '/css/reply.css');
  drupal_add_js(drupal_get_path('module', 'findit_reply') . '/js/readmore.js');
  global $pager_page_array, $pager_total, $pager_total_items;
  ctools_add_css('reply', 'findit_reply', 'css');
  ctools_include('ajax');
  ctools_include('modal');
  ctools_add_js('ajax-responder');
  ctools_modal_add_js();
  drupal_add_js(array(
    'comment-delete-modal' => array(
      'modalSize' => array(
        'type' => 'fixed',
        'width' => 500,
      ),
      'animation' => 'fadeIn',
    ),
  ), 'setting');

  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = array();
  $parameters['store_id'] = $commerce_store->unique_store_id;
  $parameters['published_only'] = 1;
  if (isset($_GET['id'])) {
    $parameters['id'] = $_GET['id'];
  }
  if (empty($_GET['sort_order'])) {
    $sort_order = 'created';
  }
  else {
    $sort_order = $_GET['sort_order'];
  }
  $data = finditreply::index('*', $parameters, $page, 10, $sort_order, 'DESC');
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];

  $output = '';
  $len = count($data['data']);
  foreach ($data['data'] as $key => $review) {
    $output .= reply_post_html($review, $commerce_store->unique_store_id);
  }
  $form_states = array();
  $form = drupal_get_form('findit_reply_sortdata_form', $form_states);
  $reply_sort = drupal_render($form);
  return '<div class="row column-heading no-gutters"><div class="col-md-6 nopadding">
<h1>Customer Reviews <span>(Showing ' . $len . ' of ' . number_format(reset($pager_total_items)) . ' Reviews)</span></h1>
</div><span class="seller-order-sort pull-right col-sm-4 nopadding">' . $reply_sort . '</span></div>' . $output . theme('pager');
}

/**
 * Callbck function for product reviews.
 *
 * @param object $commerce_store
 *   Commerce store object.
 */
function findit_reply_product_view($commerce_store) {
  global $pager_page_array, $pager_total, $pager_total_items;
  // theme('fivestar_static', array('widget' => array('name' => 'default1')));
  ctools_add_css('reply', 'findit_reply', 'css');
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();
  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = array();
  if (isset($_GET['deleted']) && $_GET['deleted'] != '') {
    $parameters['deleted'] = 1;
  }

  $parameters['product_review'] = $commerce_store->unique_store_id;
  $parameters['published_only'] = 1;
  if (isset($_GET['id'])) {
    $parameters['id'] = $_GET['id'];
  }

  $data = finditreply::index('*', $parameters, $page, 5, 'created', 'DESC');
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];

  $output = '';
  $len = count($data['data']);
  foreach ($data['data'] as $key => $review) {
    $output .= reply_post_html($review, $commerce_store->unique_store_id, 1);
  }
  return $output . theme('pager');
}

/**
 * Function for formatting reviews data in structured format.
 */
function reply_post_html($review, $store_id = 0, $product = 0) {
  $report_to_findit = '';
  $image = '';
  foreach ($review['attacted_images'] as $row) {
    $image .= '<a href="' . $row['image'] . ' "rel="lightbox" >' . '<img src="' . $row['thumbnail'] . '" alt="' . $review['subject'] . '">'
        . '</a>';
  }

  if ($review['children_total'] == 0) {
    $comments = '0 ' . t('comment') . ' <i class="triangle"></i>';
  }
  else {
    $attr = array('attributes' => array('class' => 'use-ajax'), 'html' => TRUE);
    $comments = l($review['children_total'] . ' ' . t('Comments') . ' <i class="triangle"></i>', 'review/nojs/' . $review['id'] . '/comment', $attr);
  }
  $attr = array('attributes' => array('class' => 'ctools-use-modal'));

  $review_share = !user_access('review findit access') ? '<div class="socialshare col-sm-2">' . _review_product_items($review) . '</div>' : '';
  $ptitle = '';
  $reply_block = '';
  if ($product) {
    drupal_set_title(t('Reviews & Ratings'));
    $url = variable_get('marketplace_url') . '/api_market/merchant-seller-info-service/merchant/' . $review['sku'];

    // Parameters to pass to URL.
    $data = 'type&value=' . $review['sku'];
    $options = array(
      // HTTP Request Type.
      'method' => 'GET',
      'data' => $data,
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
    );

    $result = drupal_http_request($url, $options);
    $seller_info = drupal_json_decode($result->data);
    $result_title1 = $seller_info['product_title'];
    $result_title_link1 = variable_get('marketplace_url') . '/' . $seller_info['product_url_link'];

    if (!empty($seller_info['commerce_price_amount'])) {
      $commerce_price_amount = $seller_info['commerce_price_amount'];
    }
    if (!empty($seller_info['field_default_price_value']) && $seller_info['field_default_price_value'] != 0) {
      $field_default_price_value = 'RM ' . $seller_info['field_default_price_value'];
    }
    else {
      $commerce_price_amount = '<span>' . t('Out Of Stock') . '</span>';
    }
    $field_default_rating_value = $seller_info['field_default_rating_value'] * 20;
    $total_review = $seller_info['total_review'];
    $ptitle .= '<div class="col-xs-12 col-sm-1 nopadding text-center image-wrap"><img src = "' . $review['parent_image'] . '"></div>';

    $ptitle .= '<div class="col-xs-5 col-sm-5 pdetail-options">
            <h3 class="column-title">' . l($result_title1, $result_title_link1, array('attributes' => array('target' => '_blank'))) . '</h3>
            <div class="rating rateit pull-left">' . theme('fivestar_static', array('rating' => $field_default_rating_value)) . '<span class="date">(' . $total_review . ' reviews)</span>
                    </div>
            <div class="clearfix">
           
            </div>';
    $ptitle .= '</div>';
  }
  else {
    drupal_set_title(t('Reviews & Ratings'));
    $reply = '';
    if (!user_access('review findit access')) {
      $form = drupal_get_form('_reply_merchant_reply_form', $review['id'], $store_id);
      $reply = drupal_render($form);
      // Make link of report to findit.
      $report_to_findit = '<span class="report-to-findit-' . $review['id'] . '">';
      if ($review['raised_to_findit_status'] == '0') {
        $report_to_findit .= t('Raised to findit(Pending)');
      }
      elseif ($review['raised_to_findit_status'] == '1') {
        $report_to_findit .= t('Raised to findit(Accepted)');
      }
      elseif ($review['raised_to_findit_status'] == '2') {
        $report_to_findit .= t('Raised to findit(Ignored)');
      }
      else {
        $report_to_findit .= '<span class="report-to-findit-' . $review['id'] . '">' . l(t('Raise to findit'), 'report-to-findit/nojs/' . $review['id'] . '/' . $review['store_id'], array('attributes' => array('class' => array('ctools-use-modal', 'reject')))) . '</span>';
      }
      $report_to_findit .= '</span>';
    }
    $reply_block = '<div class="other-comments-desc clearfix"><div class="review-comments"> ' . $reply . '</div></div>';

    $order_id = commerce_order_get_ref_order_id($review['order_id']);
    if ($order_id && $order = commerce_order_load($order_id)) {
      $url = variable_get('marketplace_url') . '/api_market/merchant-seller-info-service/merchant/' . $review['sku'];

      // Parameters to pass to URL.
      $data = 'type&value=' . $review['sku'];
      $options = array(
        // HTTP Request Type.
        'method' => 'GET',
        'data' => $data,
        'headers' => array('Content-Type' => 'application/x-www-form-urlencoded'),
      );

      $result = drupal_http_request($url, $options);
      $seller_store = drupal_json_decode($result->data);
      $result_title = $seller_store['product_title'];
      $result_title_link = variable_get('marketplace_url') . '/' . $seller_store['product_url_link'];
      $ord_ref = commerce_order_load($order_id);
      $order_ref = $ord_ref->order_number;
      $t = t('ORDER') . ' (' . $order_ref . ') <span class="store-review-order-placed-on">' . t('Order Placed On') . ' ' . custom_dateformat_mFY($order->created) . '</span>';
      $ptitle = '<div class="title">' . l($t, 'admin/commerce/orders/' . $order_id, array('attributes' => array('target' => '_blank'), 'html' => TRUE)) . '</div>';
    }
  }
  if (!empty($review['subject'])) {

    $reviewdate_val = '• <span class="date">' . custom_dateformat_mFY($review['created']) . '</span>•';
  }

  if ($review['is_anonymous']) {
    $review['user_name'] = '<span>' . $review['name'] . '</span>';
  } else {
    $review['user_name'] = l( $review['name'], variable_get('marketplace_url') . 'user/' . $review['uid'] . '/product-reviews');
  }

  $results['review'] = $review;
  $results['reviewdate_val'] = $reviewdate_val;
  $results['ptitle'] = $ptitle;
  $results['image'] = $image;
  $results['comments'] = $comments;
  $results['review_share'] = $review_share;
  $results['reply_block'] = $reply_block;
  $results['report_to_findit'] = $report_to_findit;
  $results['result_title_link'] = $result_title_link;
  $results['result_title'] = $result_title;

  if (arg(0) == 'store' && is_numeric(arg(1)) && arg(2) == 'review' && is_null(arg(3))) {
    $output = theme('seller_store_review', array('data' => $results));
  }
  else {
    $output = theme('seller_product_review', array('data' => $results));
  }
  /*
   * Reply removed:
   *  <div class="other-comments-desc clearfix"><div class="review-comments"> '
   * . $reply . '</div></div>
   */
  return $output;
}

/**
 * Function for formatting review comments data in structured format.
 */
function reply_post_comment_html($review) {
  $attr_nocolor = array('attributes' => array('class' => 'ctools-use-modal'));
  global $user;
  $report_to_findit = '';
  $store_id = _get_store_unique_id($user->uid);
  if ($review['status'] == 0 && $review['type'] == 'store' && $store_id == $review['store_id'] && $review['review_author'] == $store_id) {
    $edit_link = l(t('Edit'), 'review/nojs/' . $review['id'] . '/edit', $attr_nocolor);
    $delete_link = l(t('Delete'), 'review/nojs/' . $review['id'] . '/delete', array('attributes' => array('class' => array('ctools-use-modal', 'ctools-modal-comment-delete-modal'))));
  }
  else {
    if ($review['type'] == 'store' && (!isset($review['review_author']) || $review['review_author'] != $store_id)) {
      if (!user_access('review findit access')) {
        // Make link of report to findit.
        $report_to_findit = '<div class="report-to-findit-' . $review['id'] . '">';
        if ($review['raised_to_findit_status'] == '0') {
          $report_to_findit .= t('Raised to findit(Pending)');
        }
        elseif ($review['raised_to_findit_status'] == '1') {
          $report_to_findit .= t('Raised to findit(Accepted)');
        }
        elseif ($review['raised_to_findit_status'] == '2') {
          $report_to_findit .= t('Raised to findit(Ignored)');
        }
        else {
          $report_to_findit .= '<div class="report-to-findit-' . $review['id'] . '">' . l(t('Raise to findit'), 'report-to-findit/nojs/' . $review['id'] . '/' . $review['store_id'], array('attributes' => array('class' => array('ctools-use-modal', 'reject')))) . '</div>';
        }
        $report_to_findit .= '</div>';
      }
    }

    $edit_link = '';
    $delete_link = '';
  }

  $output = '<div class="other-comments" cid="' . $review['id'] . '" id="review-main-thread-' . $review['id'] . '">
        <div class="col-sm-1 nopadding"><div class="reviewer-dp"><img src="' . $review['image'] . '" alt="' . $review['name'] . '"></div></div>
        <div class="other-comments-desc">
          <a class="pull-left">' . $review['name'] . '</a>
          <span class="review-date">' . custom_dateformat_mFY($review['created']) . '</span>
            <div class="review-main-body comment more">
                <p>' . $review['description'] . '</p>
            </div>
            <div class="review-toggle-section related-cmnt-nav">
                <span class="thumb-yes-no pull-left">
                  <a>
                    <i class="icon-Yes"></i>
                    yes(' . $review['like'] . ')
                  </a>
                  <a>
                    <i class="icon-No"></i>
                    no(' . $review['dislike'] . ')
                  </a>
                </span>
                <div class="review-report-abuse pull-left">Reported as Abuse (' . $review['report'] . ') </div>' .
      $report_to_findit
      . '<div class="edit-own-reply"> ' . $edit_link . ' </div>
                <div class="delete-own-reply">' . $delete_link . '</div>
            </div>
            <div class="review-reply-body-wrap" >
                <ul class="review-reply-body-92"></ul>

            </div>
        </div>
    </div>';
  return $output;
}

/**
 * Callback function for reviews details page.
 *
 * @param int $id
 *   Review id.
 */
function findit_reply_view_reviews($id) {
  ctools_add_css('reply', 'findit_reply', 'css');
  $data = finditreply::index('*', array('id' => $id, 'show_deleted' => 1), 0, 20);
  $reply = $data['data'][0];
  $description = reply_post_html($reply);
  return $description;
}

/**
 * Function for formatting timestamp into date format.
 *
 * @param int $timestamp
 *   Timestamp to be converted into date.
 *
 * @return date
 *   Formatted date.
 */
function custom_dateformat_mFY($timestamp) {
  $date_val = date('d F Y', $timestamp);
  return $date_val;
}

/**
 * Callback function for raise to findit.
 */
function report_to_findit_callback($ajax, $review_id) {
  if ($ajax) {
    ctools_include('ajax');
    ctools_include('modal');

    $form_state = array(
      'ajax' => TRUE,
      'title' => t('Raise to Findit'),
    );
    $form_state['build_info']['args'] = array($review_id);

    // Create a form in a modal popup.
    $output = ctools_modal_form_wrapper('report_to_findit_form', $form_state);
    if (isset($form_state['reported']) && $form_state['reported'] == TRUE) {
      $output = array();
      // If item has been successfully returned.
      $title = t('Raised to findit');
      $text = '<div class="raise-to-findit-confirm">' . t('Review has been raised to findit.') . '</div>';
      $text .= '<div>' . t('You will receive a notification once it is approved by findit user.') . '</div>';
      $output[] = ctools_modal_command_display($title, $text . ctools_ajax_text_button(t('CLOSE'), 'modal_forms/nojs/dismiss', t('Close'), 'raise-to-findit-confirm-close'));
      $output[] = ajax_command_html('.report-to-findit-' . $review_id, t('Raised to findit(Pending)'));
    }
    elseif ($form_state['executed'] == TRUE && $form_state['reported'] != TRUE) {
      $output = array();
      // If item has been successfully returned.
      $title = t('Raised to findit');
      $text = '<div class="raise-to-findit-confirm">' . t('Error occurred.') . '</div>';
      $text .= '<div>' . t('You will receive a notification once it is approved by findit user.') . '</div>';
      $output[] = ctools_modal_command_display($title, $text . ctools_ajax_text_button(t('CLOSE'), 'modal_forms/nojs/dismiss', t('Close'), 'raise-to-findit-confirm-close'));
    }

    // If the form has been submitted, there may be additional instructions
    // such as dismissing the modal popup.
    if (!empty($form_state['ajax_commands'])) {
      $output[] = $form_state['ajax_commands'];
    }

    // Return the ajax instructions to the browser via ajax_render().
    print ajax_render($output);
    drupal_exit();
  }
  else {
    return drupal_get_form('report_to_findit_form', $review_id);
  }
}

/**
 * Function for getting raise to findit reasons.
 */
function findit_get_report_to_findit_reasons() {
  $report_to_findit_reasons = array();
  $reasons = variable_get('report_to_findit_reasons', '');
  // Explode reasons by new line.
  $reasons_array = explode(PHP_EOL, $reasons);
  foreach ($reasons_array as $reason) {
    $reason_trimmed = trim($reason);
    $reason_key = str_replace(' ', '_', $reason_trimmed);
    // Store same key and value so that in DB the reason is saved as it is.
    $report_to_findit_reasons[$reason_key] = $reason_trimmed;
  }
  return $report_to_findit_reasons;
}

/**
 * Form builder function for report to findit form.
 */
function report_to_findit_form($form, &$form_state, $review_id) {
  // Get report to findit reasons as these are configurable.
  $report_to_findit_reasons = findit_get_report_to_findit_reasons();
  $form['report_to_findit_reasons'] = array(
    '#type' => 'select',
    '#title' => t('Filter'),
    '#options' => $report_to_findit_reasons + array('other' => t('Other')),
  );
  $form['report_to_findit_reason_other'] = array(
    '#type' => 'textarea',
    '#title' => t('Other Reason'),
    '#states' => array(
      'visible' => array(':input[name="report_to_findit_reasons"]' => array('value' => "other")),
      'required' => array(':input[name="report_to_findit_reasons"]' => array('value' => "other")),
    ),
  );
  $form['review_id'] = array(
    '#type' => 'hidden',
    '#value' => $review_id,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  $form['cancel'] = array(
    '#markup' => l(t('Cancel'), '#', array(
      'attributes' => array(
        'class' => 'ctools-close-modal',
      ),
      'external' => TRUE,
    )
    ),
  );
  return $form;
}

/**
 * Validate function for report to findit form.
 */
function report_to_findit_form_validate($form, &$form_state) {
  if ($form_state['values']['report_to_findit_reasons'] == 'other' && empty(trim($form_state['values']['report_to_findit_reason_other']))) {
    form_set_error('report_to_findit_reason_other', t('Please provide other reason for raise to findit.'));
  }
}

/**
 * Submit function for report to findit form.
 */
function report_to_findit_form_submit($form, &$form_state) {
  // Check if cancel is being clicked.
  if ($form_state['clicked_button']['#value'] != $form_state['values']['cancel']) {
    // Save message blast on maketplace.
//    $op = 'create';
//    $data = drupal_json_encode(array(
//      'type' => 'report_to_findit',
//      'data' => array(
//        'review_id' => $form_state['values']['review_id'],
//        'report_to_findit_reasons' => $form_state['values']['report_to_findit_reasons'],
//        'report_to_findit_reason_other' => $form_state['values']['report_to_findit_reason_other'],
//    )));
//    $data = 'data=' . $data . '&op=create';
//    $service = new QueueServicesConnector;
//    $service->queueapiinvoke($data, $op);
    // $unique_store_id = merchant_unique_store_id($user_id);
    $data = array(
      'review_id' => $form_state['values']['review_id'],
      'report_to_findit_reasons' => $form_state['values']['report_to_findit_reasons'],
      'report_to_findit_reason_other' => $form_state['values']['report_to_findit_reason_other'],
    );
    $options = array(
      'method' => 'POST',
      'headers' => array('Content-Type' => 'application/json'),
    );
    if (!empty($data)) {
      $options['data'] = drupal_json_encode($data);
    }
    $url = variable_get('marketplace_url') . '/api_market/report_to_findit';
    $review_data = drupal_http_request($url, $options);

    $form_state['reported'] = TRUE;
    $review = drupal_json_decode($review_data->data);
    // Send email to the merchant.
    $vars = array();
    $store = findit_reply_order_get_store_unique_id($review['store_id']);
    $vars['account'] = user_load($store->uid);
    $vars['pet_name'] = variable_get('raise_to_findit_merchat_email', 'raise_to_findit_merchat_email');
    $vars['review'] = $review;
    $vars['review']['review_page_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE));
    $vars['review']['review_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE, 'query' => array('id' => $review['id'])));
    $vars['review']['raised_to_findit_reason'] = $form_state['values']['report_to_findit_reasons'];
    $vars['review']['raised_to_findit_reason_other'] = $form_state['values']['report_to_findit_reason_other'];
    findit_reply_send_pet($vars);
  }
}

/**
 * Callback function for raised to findit reviews.
 */
function findit_reported_to_findit_view($user) {
  ctools_include('ajax');
  ctools_include('modal');
  ctools_modal_add_js();
  drupal_add_js(drupal_get_path('module', 'findit_reply') . '/js/findit_reply_admin.js');
  drupal_add_js('http://cdn.jsdelivr.net/jquery.validation/1.14.0/jquery.validate.min.js', 'external');
  drupal_add_js(drupal_get_path('module', 'findit_form_validation') . '/js/findit_form_validation.js');
  global $pager_page_array, $pager_total, $pager_total_items;
  $page = isset($_GET['page']) ? $_GET['page'] : 0;
  $parameters = _findit_reply_get_parameters('', 'reported_to_findit');
  $sort = $parameters['sort'];
  $direction = $parameters['direction'];
  unset($parameters['sort']);
  unset($parameters['direction']);

  $data = finditreply::index('*', $parameters, $page, 20, $sort, $direction);
  $pager_page_array[0] = $page;
  $pager_total[0] = $data['pages']['total'];
  $pager_total_items[0] = $data['items']['total'];
  $rows = array();
  $attr = array('attributes' => array('class' => 'ctools-use-modal'));
  $attr_tb = array('attributes' => array('target' => '_blank'));
  foreach ($data['data'] as $review) {
    $row = array();
    $link = array();
    if ($review['parent'] != 0) {
      $link[] = l(t('View'), 'review/' . $review['parent'] . '/view', $attr_tb);
    }
    else {
      $link[] = l(t('View'), 'review/' . $review['id'] . '/view', $attr_tb);
    }
    if ($review['raised_to_findit_status'] == 0 && $review['deleted'] == 0) {
      $link[] = '<div class="findit-ignore-raise-to-findit-' . $review['id'] . '">' . l(t('Ignore'), 'javascript:void(0)', array(
        'external' => TRUE,
        'attributes' => array(
          'class' => array('findit-ignore-report-to-findit-link'),
          'review_id' => $review['id'],
        ),
      )
          ) . '</div>';
      $link[] = l(t('Unpublish'), 'review/nojs/' . $review['id'] . '/deleted/0/report_to_findit', $attr);
    }
    elseif ($review['raised_to_findit_status'] == 1) {
      $link[] = t('Unpublished');
    }
    $row[] = custom_dateformat_mFY($review['created']);
    if ($review['field_review_by_value'] != 'merchant') {
      $row[] = $review['field_user_first_name_value'];
      $row[] = $review['mail'];
    }
    else {
      $row[] = _get_seller_name_by_unid($review['field_author_id_value']);
      $row[] = '';
    }
    $subject = $review['parent'] == 0 ? '<b>' . $review['field_review_subject_value'] . '</b><br/>' : '';
    $row[] = $subject . substr($review['field_review_description_value'], 0, 200) . '...';
    $p = $review['parent'] == 0 ? '' : $review['field_review_subject_value'];
    $row[] = $p;
    $row[] = $review['parent'] == 0 ? ('seller review') : t('comment on seller review');
    $keys = explode('--', $review['field_review_order_refrence_value']);
    $row[] = isset($keys[1]) ? $keys[1] : '';
    $row[] = l(_get_seller_name_by_unid($review['unique_store_id']), $review['parent_url'], $attr_tb);
    $status = $review['status'] == 0 ? t('Unpublished') : t('Published');
    $row[] = $review['deleted'] == 0 ? $status : t('Deleted');
    // Get reject reason text.
    if ($review['raise_to_findit_reason'] == 'other') {
      $row[] = (!empty($review['raise_to_findit_desc']) ? t('Other') . ' - ' . $review['raise_to_findit_desc'] : t('Other'));
    }
    else {
      $row[] = str_replace('_', ' ', $review['raise_to_findit_reason']);
    }
    $row[] = implode(' | ', $link);
    $rows[] = $row;
  }
  $header = array(
    t('Date submitted'),
    t('Submitted by'),
    t('Customer Email'),
    t('Review text'),
    t('Parent Review'),
    t('Type (Review/Comment)'),
    t('Sku'),
    t('Seller Details'),
    t('Status'),
    t('Reason to raise'),
    t('Actions'),
  );

  // Add filter form.
  $form_states = array();
  $form = drupal_get_form('findit_reply_view_form', $form_states);

  return drupal_render($form) . theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No reviews found'),
  )
      ) . theme('pager');
}

/**
 * AJAX callback for ignore of raise to findit.
 */
function findit_raise_to_findit_ignore($review_id) {
  $values['action'] = 'raise_to_findit_ignore';
  $values['review_id'] = $review_id;
  $review = finditreply::update($review_id, $values);
  $vars = array();
  $store = findit_reply_order_get_store_unique_id($review['store_id']);
  $vars['account'] = user_load($store->uid);
  $vars['pet_name'] = variable_get('report_to_findit_ignore_mail_to_merchant', 'report_to_findit_ignore_mail_to_merchant');
  $vars['review'] = $review;
  $vars['review']['review_page_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE));
  $vars['review']['review_url'] = url('/store/' . $store->store_id . '/review/product', array('absolute' => TRUE, 'query' => array('id' => $review['id'])));
  findit_reply_send_pet($vars);
  print 'success';
}

/**
 * Function for getting email id of store owner.
 */
function _get_user_email_from_store_id($unique_store_id) {
  $query = db_select('commerce_store', 'cs');
  $query->join('users', 'u', 'u.uid = cs.uid');
  $query->fields('u', array('mail'));
  $query->condition('cs.unique_store_id', $unique_store_id);
  $result = $query->execute()->fetchAssoc();
  return $result['mail'];
}

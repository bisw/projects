<?php

/**
 *
 * @file - page callbacks
 *
 */

 /*
  * page callback for update.
  * 
  * @param object $return
  *
  * @return string
  */
function findit_order_return_view($return) {
    // build form
  $output ='';
  if (arg(2) != 'view') {
    $form_states = array('#entity' => $return);
    $form = drupal_get_form('findit_order_return_form', $return);
    $output = drupal_render($form);
  }

   //return render for view.
   //$output .= commerce_rma_return_view($return,'administrator');
   //$output  .= '<h3>' . t('Return History') . '</h3>';
   //$output .= find_return_history($return->return_id);
   return $output;
}


 
/**
 * callback for build form for return processing.
 */
function findit_order_return_form($form, &$form_state, $return) {

  $statuses = commerce_return_status_options_list_flat();
  $options =  return_get_next_available_status($statuses, $return);
  if (count($options) <= 1 && (user_access('return merchent access') && !user_access('return findit access') )) {
    return $form;
  }
  $form['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#description' => t('Please select status'),
    '#default_value' => $return->status,
    '#chosen' => FALSE,
    '#options' =>  $options,
  );
  $form['log'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#title' => t('Comments'),
    '#required' => TRUE,
  );
  $form['return_id'] = array(
    '#type' => 'hidden',
    '#value' => $return->return_id,
    );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Return order')
  );
  return $form;
}


/**
 * Impliment hook_form_submit.
 *
 */
function findit_order_return_form_submit($form, &$form_states) {
    $data = $form_states['values'];

    $return =  commerce_return_load($data['return_id']);
    if ($data['status'] == 'rejected' && $return->status != 'rejected') {
      $options = array('query' => array('log' => $data['log'], 'destination' => $_GET['destination']) );
      unset($_REQUEST['destination']);
      unset($_GET['destination']);
      $form_states['redirect'] = url('returns/' . $data['return_id'] . '/reject', $options);
      //$form_states['rebuild'] = TRUE;
      drupal_goto('returns/' . $data['return_id'] . '/reject', $options);
    }
    else {
      //Update return status and log.
      commerce_return_status_update($return, $data['status'], FALSE, TRUE, $data['log']);
      drupal_set_message(t('Return has been updated.'));
    }
}


function findit_order_return_reject_form($form, &$form_states, $return) {

  if ($return->status != 'received') {
    drupal_goto('returns/' . $return->return_id);
  }

  $options = array('' => '--Select--');
  $reason = preg_split('/\r\n|\r|\n/', variable_get('return_rejected_order_reason', ''));
  foreach($reason as $val) {
    $options[$val] = $val;
  }

  $form['reason'] = array(
    '#type' => 'select',
    '#title' => t('Reason'),
    '#description' => t('Please select status'),
    '#chosen' => FALSE,
    '#default_value' => '',
    '#required' => TRUE,
    '#options' =>  $options,
  );
  $form['reason_other'] = array(
    '#type' => 'textfield',
    '#rows' => 2,
    '#title' => t('Enter other reason'),
    '#description' => 'Please enter reason if you choose "other" option from Reason option.',

  );
  $form['reject_image'] = array(
    '#type' => 'managed_file',
    '#title' => t('Image'),
    '#description' => t('Return product image '),
    '#upload_location' => "public://rejected_order",
    '#upload_validators' => array(
      'file_validate_extensions' => array('gif png jpg jpeg'),
      // Pass the maximum file size in bytes
      'file_validate_size' => array(2*1024*1024),
    ),
  );
  $form['log'] = array(
    '#type' => 'textarea',
    '#rows' => 2,
    '#title' => t('Note'),
   // '#required' => TRUE,
  //  '#default_value' => isset($_REQUEST['log']) ? check_plain($_REQUEST['log']) : '',
  );
  $form['return_id'] = array(
    '#type' => 'hidden',
    '#value' => $return->return_id,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Reject order')
  );
  return $form;
}



/**
 * Impliment hook_form_submit.
 *
 */
function findit_order_return_reject_form_submit($form, &$form_states) {
  $data = $form_states['values'];
  $return =  commerce_return_load($data['return_id']);
  $return->status = 'rejected';
  $return->log =   $data['log'];
  $reason = $data['reason'];
  if ($data['reason'] == 'other' && $data['reason_other'] != '') {
    $reason = $data['reason_other'];
  }

  $return->field_return_reject_reason[LANGUAGE_NONE] = array(array('value' => $reason));
  $return->field_rejection_date[LANGUAGE_NONE] = array(array('value' => time()));
  if ($data['reject_image']) {
    $file = file_load($data['reject_image']);
    $return->field_rejection_image[LANGUAGE_NONE] = array((array) $file);
  }
  commerce_return_save($return);
  drupal_goto('returns/' . $data['return_id']);
}

/**
 * Get history of return by return_id
 *
 * @param int $retrun_id
 *  - id of return.
 *
 * @return string
 *  - Return history as table format.

 */
function find_return_history($retrun_id = 0) {
  $revisions = db_select('commerce_return_revision', 'revision')
    ->fields('revision')
    ->condition( 'return_id', $retrun_id)
    ->orderBy('revision_timestamp')
    ->execute();

  $rows = array();
  $statuses = commerce_return_status_options_list();
  $all_status = array();
  foreach ($statuses as $data) {
    $all_status = array_merge($all_status, $data);
  }
  $i = 1;
  $m_statuses = variable_get('return_status_not_merchant', array());

  $is_merchant = user_access('return merchent access') && !user_access('return findit access');
  foreach ($revisions as $key => $revision) {
    if ( $is_merchant) {
      if (in_array($revision->status, $m_statuses)) {
        continue;
      }
    }
    $name = t("Customer");
    if ($revision->revision_uid != 0) {
      $account = user_load($revision->revision_uid);
      $name = $account->name;
    }
    $updated_date = date("d/m/Y", $revision->revision_timestamp);
    $updated_time = date("H:i", $revision->revision_timestamp);
    $ret_status = "<span class=status-$revision->status>".$all_status[$revision->status]."</span>";
    $row = array(
      $updated_date ,
      $updated_time,
      $ret_status,
      $revision->log,
    );
    $i++;
    $rows[] = $row;
  }
  $header = array(
    t('Updated Date'),
    t('Updated Time'),
    t('Status'),
    t('Additional Note'),
  );
  return theme('table', array('rows' => $rows, 'header' => $header));
}

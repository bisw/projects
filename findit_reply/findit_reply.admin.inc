<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Admin Configuration form Admin ID
 */

function findit_admin_id_config_settings($form,$form_state){
    $form['admin_dummy_id'] = array(
    '#type' => "textfield",
    '#title' => t("Enter Admin Id"),
    "#description" => t("Enter Admin Id for dummy user"),
    '#default_value' => variable_get('admin_dummy_id', array()),
  );
    
  return system_settings_form($form);
}

/**
 * Function of reasons for report to findit of reviews.
 */
function report_to_findit_reasons($form, $form_state) {
  $form['report_to_findit_reasons'] = array(
    '#type' => 'textarea',
    '#default_value' => variable_get('report_to_findit_reasons', ''),
    '#title' => t('Report to findit reasons'),
    '#description' => t('Enter report to findit reasons separated by new lines.'),
  );
  return system_settings_form($form);
}

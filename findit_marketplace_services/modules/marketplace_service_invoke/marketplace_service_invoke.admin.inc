<?php

/**
 * @file
 * Gives the admin setting form.
 */

/**
 * 
 * @param type $form
 * @param type $form_state
 * @return type
 */
function marketplace_service_invoke_settings_form($form, $form_state) {
  $form['findit_merchant_api_order_url'] = array(
    '#type' => 'textfield',
    '#title' => t('URL for order Sinking'),
    '#default_value' => variable_get('findit_merchant_api_order_url', NULL),
    '#size' => 60,
  );

  return system_settings_form($form);
}

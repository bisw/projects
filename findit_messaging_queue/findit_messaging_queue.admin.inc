<?php

/**
 * @file
 * The findit_messaging_queue.admin.module
 *
 * This is stipr admin setting form whhere strip api public
 * and private key needs to be included.
 * @return $form
 */

function findit_rabit_mq_conf_admin_setting_form() {


  $form = array();

  $form['rab_mq_host'] = array(
  '#type' => 'textfield',
  '#title' => t('Host'),
  '#description' => t('Please Enter Host.'),
  '#default_value' => variable_get('rab_mq_host','localhost'),
  );
  $form['rab_mq_port'] = array(
  '#type' => 'textfield',
  '#title' => t('Port'),
  '#description' => t('Please Enter Port.'),
  '#default_value' => variable_get('rab_mq_port','5672'),
  );
  $form['rab_mq_username'] = array(
  '#type' => 'textfield',
  '#title' => t('Username'),
  '#description' => t('Please Enter Username.'),
  '#default_value' => variable_get('rab_mq_username','guest'),
  );
  $form['rab_mq_password'] = array(
  '#type' => 'textfield',
  '#title' => t('Password'),
  '#description' => t('Please Enter Password.'),
  '#default_value' => variable_get('rab_mq_password','guest'),
  );
  $form['rab_mq_vhost'] = array(
  '#type' => 'textfield',
  '#title' => t('VHost'),
  '#description' => t('Please Enter vhost.'),
  '#default_value' => variable_get('rab_mq_vhost','/'),
  );
  return system_settings_form($form);

}

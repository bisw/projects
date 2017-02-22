<?php

/**
 * Admin Configuration form
 */
function findit_marketplace_services_settings() {
  $form['service_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Service name'),
    '#required' => TRUE,
    '#description' => t('The human-readable name.'),
  );
  $form['service_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Service url'),
    '#required' => TRUE,
    '#description' => t('The URL of the web service.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['servise_list'] = array(
    '#type' => 'markup',
    '#markup' => findit_services_list(),
  );

  return $form;
}

/**
 * Submit callback of the web service description form.
 */
function findit_marketplace_services_settings_submit($form, &$form_state) {
  $values = $form_state['values'];
  if (!empty($values)) {
    $query = db_merge('findit_services_settings')
        ->key(array(
          'service_name' => $values['service_name'],
        ))
        ->fields(array(
          'service_name' => $values['service_name'],
          'service_url' => $values['service_url'],
        ))
        ->execute();
    drupal_set_message(t('Your changes has been saved.'));
  }
}

function findit_services_list() {
  $rows = array();
  $fetch_all = db_select('findit_services_settings', 'fss')
      ->fields('fss')
      ->execute()
      ->fetchAll();
  foreach ($fetch_all as $key => $value) {
    $row['name'] = $value->service_name;
    $row['url'] = $value->service_url;
    $row['delete'] = l(t("Delete"), 'findit-delete-url/' . $value->service_name);
    $rows[] = $row;
  }
  if (count($rows)) {
    $header = array(t('Service name'), t('Service Url'), t("Action"));
    $table = array('header' => $header, 'rows' => $rows, 'attributes' => array(), 'caption' => NULL, 'colgroups' => array(), 'sticky' => TRUE, 'empty' => '');
    return theme_table($table);
  }
  return t("No Services Added Yet");
}

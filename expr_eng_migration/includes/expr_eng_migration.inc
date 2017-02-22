<?php

function expr_eng_migration_get_config_migration_form($form, &$form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Config'),
  );  
  return $form;
}

function expr_eng_migration_get_config_migration_form_submit($form, &$form_state) {
  expr_eng_migration_update_config();
}

function expr_eng_migration_update_config() {
  db_set_active('expressionEng');

  // Fetch datus from expressionEngine database
  $exprFields = expr_eng_migration_fetch_fields();  
  $channels = expr_eng_migration_fetch_channels();  
  $cats = expr_eng_migration_fetch_categories();
  $categories = expr_eng_migration_get_categories();
  $terms = expr_eng_migration_fetch_taxonomy_terms();

  db_set_active();

  // Update database changes in drupal.
  // Update vocabulary.
  expr_eng_migration_create_vocabulary($cats);
  expr_eng_migration_create_taxonomy_term($terms, $categories);
  $fields = expr_eng_migration_create_base_fields($exprFields);
  foreach($channels as $channel) {
    expr_eng_create_migration_content_type($channel);
    expr_eng_create_migration_field_instance($exprFields, $channel);
  }

  drupal_set_message('Updated');
}

function expr_eng_migration_update_content_form($form, &$form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Content'),
  );
  return $form;
}

function expr_eng_migration_update_content_form_submit($form, &$form_state) { 
  expr_eng_migration_update_content();
}

function expr_eng_migration_update_content() {
  db_set_active('expressionEng');
  $data = expr_eng_migration_fetch_channels_data();

  db_set_active();
  expr_eng_migration_create_node($data);
}
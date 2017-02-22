<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

class YourTangoProfileMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    //$this->dependencies = array('YourTangoUser');
    //Add a field to your data row for each profile field you need to migrate
    $source_fields = array(
      'uid' => t('User ID'),
      'full_name' => t('The full name of the user'),
    );
    // Select fields from the Drupal 6 user table.
    $connection = yt_migration_db_connection();
    $query = $connection
        ->select('users', 'u')
        ->fields('u', array('uid'))
        ->condition('u.uid', 1, '>'); // You do not want to mess with User-1.
    // Set source and destination.
    $this->source = new MigrateSourceSQL($query, $source_fields, NULL, array('map_joinable' => FALSE));
    $this->destination = new MigrateDestinationProfile2('main'); // The machine name of your profile.
    // Set up database maping.
    $this->map = new MigrateSQLMap($this->machineName, array(
      'uid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique User ID',
        'alias' => 'u',
      )
        ), MigrateDestinationProfile2::getKeySchema()
    );
    // Connecting the profile2 to the user:
//    $this->addFieldMapping('uid', 'uid')
//            ->sourceMigration('YourTangoUser')
//            ->description(t('The assignment of profile2-items to the respective user profile'));
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('field_address:name_line', 'full_name');
    $this->addUnmigratedSources(array('nonmember', 'memberorg', 'memberindiv'));
  }

  public function prepareRow($current_row) {
    $source_id = $current_row->uid;
    $connection = rsb_migration2_get_source_connection();
    $query = $connection->select('profile_values', 'pv') // The data in each profile field is stored here.
        ->fields('pv', array('value'))
        ->condition('pv.uid', $source_id, '=');
    $query->join('profile_fields', 'pf', 'pv.fid = pf.fid'); // The field names are stored here.
    $query->addField('pf', 'name');
    $query->orderBy('pf.fid', 'ASC');
    $result = $query->execute();

    // The next few lines are what make the whole thing work.
    foreach ($result as $row) {
      $field_name = str_replace('profile_', '', $row->name); // Simplify the field names.
      $current_row->$field_name = $row->value;
    }

    $current_row->full_name = $current_row->fname . ' ' . $current_row->lname;
    // This sanitizes phone numbers because cck_phone stores them as strings of numbers. This may not be absolutely necessary because cck_phone has a function to clean up numbers.
    return TRUE;
  }

}

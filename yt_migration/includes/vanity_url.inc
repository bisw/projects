<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing users from d6 to d7.
 */
class YourTangoVanityUrlMigration extends YourTangoMigration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Vanity Urls Migration';
    //$this->dependencies = array('YourTangoUsers');

    $table_name = 'url_alias';
    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('url_alias', 'ur')
        ->fields('ur', array('pid', 'src', 'dst'))
        ->condition('ur.src', 'experts/%', 'LIKE');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTable($table_name);
    $this->map = new MigrateSQLMap($this->machineName, array(
      'pid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )
        ), MigrateDestinationTable::getKeySchema($table_name)
    );

    // Mapped fields
    $this->addFieldMapping('source', 'src');
    $this->addFieldMapping('alias', 'dst');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_experts_redirect($entity, $row);
  }

}

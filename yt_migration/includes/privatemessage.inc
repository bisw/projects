<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * @file
 * Base class for migrating privatemsg entities.
 *
 * @todo Handle deletions
 */
class YourTangoPrivateMessageMigration extends YourTangoMigration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'PM Message table data';
    $this->dependencies = array('YourTangoUsers');

    $table_name = 'pm_message';
    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('pm_message', 'pm')
        ->fields('pm', array('author', 'subject', 'body', 'format', 'timestamp', 'mid'));
    $query->addField('pm', 'mid', 'old_mid');
    $source_fields = array(
      'pm_index' => t('Fetch Whole Threads'),
    );
    $this->source = new MigrateSourceSQL($query, $source_fields);
    $this->destination = new MigrateDestinationTable($table_name);
    $this->map = new MigrateSQLMap($this->machineName, array(
      'mid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )
        ), MigrateDestinationTable::getKeySchema($table_name)
    );

    // Mapped fields
    $this->addFieldMapping('mid', 'mid');
    $this->addFieldMapping('author', 'author')->sourceMigration('YourTangoUsers');
    $this->addFieldMapping('subject', 'subject');
    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('timestamp', 'timestamp');
    $this->addFieldMapping('has_tokens')->defaultValue(0);
  }

  public function prepareRow($current_row) {
    $current_row->pm_index = yt_migration_fetch_message_thread($current_row);
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_save_message_thread($entity, $row);
  }

}

/**
 * @file
 * Base class for migrating privatemsg entities.
 *
 * @todo Handle deletions
 */

/*class YourTangoPrivateMessageIndexMigration extends YourTangoMigration {
 public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'PM Message index table data';
    $this->dependencies = array('YourTangoUsers');
    $this->dependencies = array('YourTangoPrivateMessage');

    $table_name = 'pm_index';
    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('pm_index', 'pi')
             ->fields('pi', array('mid', 'thread_id', 'is_new', 'deleted'));
    $query->addField('pi', 'uid', 'recipient');
    $query->groupBy('pi.mid');
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationTable($table_name);
    $this->map = new MigrateSQLMap($this->machineName,
      array(
		'mid' => array(
		      'type' => 'int',
		      'unsigned' => TRUE,
		      'not null' => TRUE,
		     ),
         ),
        MigrateDestinationTable::getKeySchema($table_name)
      );

    // Mapped fields
    //$this->addFieldMapping('mid', 'mid');
    $this->addFieldMapping('thread_id', 'thread_id');
    $this->addFieldMapping('recipient', 'recipient')->sourceMigration('YourTangoUsers');
    $this->addFieldMapping('is_new','is_new');
    $this->addFieldMapping('deleted','deleted');
    $this->addFieldMapping('type')->defaultValue('user');
  }

  /**
   * Our post processing method
   
  public function complete($row) {
    yt_migration_save_message_thread($row);
  }
}*/

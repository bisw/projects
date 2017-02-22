<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * @file
 * Base class for migrating privatemsg entities.
 *
 * @todo Handle deletions
 */
class YourTangoRedirectMigration extends YourTangoMigration {

  public function __construct($arguments) {
    parent::__construct($arguments);
    $this->description = 'Tango Redirect';
    $this->dependencies = array('YourTangoUsers');

    $table_name = 'field_data_field_unpublished_redirect_url';
    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('tango_redirect', 'tr')
        ->fields('tr', array('rid', 'nid', 'old_url', 'new_url', 'processed', 'time', 'published'));
    $source_fields = array(
      'entity_type' => t('Entity Type'),
      'bundle' => t('Bundle'),
      'revision_id' => t('Revision ID'),
      'field_unpublished_redirect_url_url' => t('Unpublished URL'),
      'field_unpublished_redirect_url_title' => t('Unpublished URL Title'),
      'field_unpublished_redirect_url_attributes' => t('Unpublished URL Attributes'),
      'delta' => t('Delta')
    );
    $this->source = new MigrateSourceSQL($query, $source_fields);
    $this->destination = new MigrateDestinationTable($table_name);
    $this->map = new MigrateSQLMap($this->machineName, array(
      'rid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      )
        ), MigrateDestinationTable::getKeySchema($table_name)
    );

    // Mapped fields
    $this->addFieldMapping('entity_type', 'entity_type')->defaultValue('node');
    $this->addFieldMapping('bundle', 'bundle');
    $this->addFieldMapping('deleted')->defaultValue(0);
    $this->addFieldMapping('entity_id', 'nid');
    $this->addFieldMapping('delta', 'delta');
    $this->addFieldMapping('language', 'language');
    $this->addFieldMapping('revision_id', 'revision_id');
    $this->addFieldMapping('field_unpublished_redirect_url_url', 'field_unpublished_redirect_url_url');
    $this->addFieldMapping('field_unpublished_redirect_url_title', 'field_unpublished_redirect_url_title');
  }

  public function prepareRow($current_row) {
    $node = node_load($current_row->nid);
    if (!is_object($node) || $node->type == '') {
      return FALSE;
    }
    $count_query = db_select('field_data_field_unpublished_redirect_url', 'fd');
    $count_query->fields('fd', array('entity_id'));
    $count_query->condition('fd.entity_id', $node->nid);
    $count_query->condition('fd.entity_type', $node->entity_type);
    $count_query->condition('fd.bundle', $node->type);
    $count_query->condition('fd.revision_id', $entity->vid);
    $count = count($count_query->execute());
    $current_row->entity_type = $node->entity_type;
    $current_row->bundle = $node->type;
    $current_row->language = LANGUAGE_NONE;
    $current_row->revision_id = $node->vid;
    $current_row->field_unpublished_redirect_url_url = $current_row->new_url;
    $current_row->field_unpublished_redirect_url_title = $current_row->old_url;
    $current_row->delta = $count > 1 ? $count - 1 : 0;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    db_merge('field_revision_field_unpublished_redirect_url')
        ->fields(
            array(
              'field_unpublished_redirect_url_url' => $row->new_url,
              'field_unpublished_redirect_url_title' => $row->old_url,
              'deleted' => '0'
        ))
        ->key(array(
          'delta' => $row->delta,
          'entity_id' => $row->nid,
          'entity_type' => 'node',
          'bundle' => $row->bundle,
          'revision_id' => $row->revision_id,
          'language' => LANGUAGE_NONE,
        ))
        ->execute();
  }

}

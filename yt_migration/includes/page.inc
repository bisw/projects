<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoPageMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for feature.');

    // Define the field which normally is a primary key
    $this->map = new MigrateSQLMap($this->machineName, array(
      'nid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Node ID',
        'alias' => 'n',
      )
        ), MigrateDestinationNode::getKeySchema()
    );

    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('nid', 'vid', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->condition('n.type', 'page', '=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    //$query->leftJoin('content_type_page', 'ct', 'ct.vid = n.vid');

    $query->fields('nr', array('body', 'teaser', 'format'));

    // If there is data from statistic.module, grab the view counts
    if (module_exists('statistics')) {
      $query->join('node_counter', 'nc', 'n.nid = nc.nid');
      $query->addField('nc', 'totalcount');
      $query->addField('nc', 'daycount');
      $query->addField('nc', 'timestamp');
    }
    $query->orderBy('n.nid', 'ASC');

    // Add source fields which not queried in $query, will be populated in prepareRow()
    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node')
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('page');

    // Mapping: Assign mappings TO destination fields FROM source fields.
    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('nid', 'title', 'created', 'changed', 'status', 'promote', 'sticky'));

    // Define default value for some fields
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('revision_uid', 'uid');
    $this->addFieldMapping('uid', 'uid');

    // Mapping of the body field
    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'));
    $this->addFieldMapping('body', 'body')->arguments($body_arguments);
    $this->addFieldMapping('body:format')->defaultValue('full_html');

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('field_page_title', 'metatag_title');

    // If there is data from statistic.module, map the view counts
    if (module_exists('statistics')) {
      $this->addSimpleMappings(array('totalcount', 'daycount', 'timestamp'));
    }

    // Unmapped source fields
    $this->addUnmigratedSources(array('tnid', 'translate', 'teaser', 'format'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('log', 'tnid', 'translate', 'body:summary', 'body:language', 'comment'));
  }

  public function prepareRow($current_row) {

    // Set the text format for the node.
    $current_row->format = yt_migration_get_text_format($current_row->format);

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    //Anonymous content migration condition
    if ($current_row->uid == 0) {
      $current_row->uid = 1132;
      $count_words = strip_tags($current_row->body);
      if (count(str_word_count($count_words, 1)) < 200) {
        return FALSE;
      }
    }


    // Meta fields data for the node
    $current_row->metatag_title = yt_migration_get_metatag_title($current_row->nid);
    $metatag_description = yt_migration_get_metatag_data($current_row->nid, 'description');
    $metatag_keywords = yt_migration_get_metatag_data($current_row->nid, 'keywords');
    $current_row->metatag_description = $metatag_description;
    $current_row->metatag_keywords = $metatag_keywords;

    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_node_metatag_save($entity, $row);
  }

}

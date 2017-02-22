<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoFactOfLoveMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for feature.');
    $this->dependencies = array('YourTangoUsers');
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
        ->fields('n', array('nid', 'vid', 'uid', 'title', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->condition('n.type', 'lovefact', '=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_lovefact', 'ct', 'ct.vid = n.vid');

    $query->fields('nr', array('body', 'teaser', 'format'));
    $query->addField('ct', 'field_lovefact_link_url');
    $query->addField('ct', 'field_lovefact_link_title');
    $query->addField('ct', 'field_lovefact_link_attributes');

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
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'auto_title' => t('Auto Title'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('lovefact');

    // Mapping: Assign mappings TO destination fields FROM source fields.
    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('nid', 'uid', 'created', 'changed', 'status', 'promote', 'sticky', 'title'));

    // Define default value for some fields
    //$this->addFieldMapping('title', 'auto_title')->defaultValue('');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('revision_uid', 'uid');

    // Mapping of the body field
    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'));
    $this->addFieldMapping('body:format')->defaultValue('full_html');
    $this->addFieldMapping('body', 'body')->arguments($body_arguments);

    //Add fields mapping
    $this->addFieldMapping('field_website', 'field_lovefact_link_url')->defaultValue('');
    $this->addFieldMapping('field_website:title', 'field_lovefact_link_title')->defaultValue('');
    $this->addFieldMapping('field_website:attributes', 'field_lovefact_link_attributes')->defaultValue('');

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');


    // If there is data from statistic.module, map the view counts
    if (module_exists('statistics')) {
      $this->addSimpleMappings(array('totalcount', 'daycount', 'timestamp'));
    }

    // Unmapped source fields
    $this->addUnmigratedSources(array('tnid', 'translate', 'teaser', 'format'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('log', 'tnid', 'translate', 'body:language', 'comment'));
  }

  public function prepareRow($current_row) {

    // Set the text format for the node.
    $current_row->format = yt_migration_get_text_format($current_row->format);

    //Auto title
    $limit = 10;
    $text = $current_row->body;
    $text = strip_tags($text);
    $words = str_word_count($text, 2);
    $pos = array_keys($words);
    if (count($words) > $limit) {
      $text = substr($text, 0, $pos[$limit]);
      $text = trim($text);
      $text = rtrim($text, '.');
      $text = trim($text) . '...';
    }
    $current_row->auto_title = $text;

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

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

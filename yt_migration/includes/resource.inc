<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoResourcesMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces to resource');
    $this->dependencies = array('YourTangoTaxonomyFile');
    $this->dependencies = array('YourTangoUsers');
    //$this->dependencies = array('YourTangoUser');
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
        ->condition('n.type', 'resource', '=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_resource', 'ct', 'ct.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_feature_summar', 'cs', 'cs.vid = n.vid');

    // Join with files
    $query->leftJoin('files', 'file', 'file.fid = ct.field_image_resource_fid');

    $query->fields('nr', array('body', 'teaser', 'format'));
    $query->fields('file', array('fid', 'filename', 'filepath', 'filemime', 'filesize', 'timestamp'));
    $query->addField('ct', 'field_image_resource_fid');
    $query->addField('ct', 'field_image_resource_list');
    $query->addField('ct', 'field_image_resource_data');
    $query->addField('cs', 'field_daily_dish_feature_summar_value');

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
      'image_alt' => t('Image alt field for the node'),
      'image_title' => t('Image title field for the node'),
      'exclusive_yourtango' => t('Exclusive to your tango'),
      'field_meta_canonical_url' => t('External site link'),
      'allow_index' => t('Allow index')
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('resource');

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

    // Mapping fields not need to be preprocessed (text, user reference to entity reference)
    $this->addFieldMapping('field_daily_dish_feature_summar', 'field_daily_dish_feature_summar_value');
    $this->addFieldMapping('field_meta_exclusive_content', 'exclusive_yourtango');
    $this->addFieldMapping('field_meta_canonical_url', 'field_meta_canonical_url');

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('field_page_title', 'metatag_title');

    // Mapping file fields
    $this->addFieldMapping('field_image_blog:alt', 'image_alt');
    $this->addFieldMapping('field_image_blog:title', 'image_title');
    $this->addFieldMapping('field_image_blog', 'fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_image_blog:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_image_blog:ignore_case')->defaultValue(TRUE);
    $this->addFieldmapping('field_image_blog:preserve_files')->defaultValue(TRUE);

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

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    /** Exclusive to your tango Logic* */
    $exclusive = yt_migration_tango_exclusive($current_row->nid);
    //if (is_object($exclusive)) {
    $current_row->exclusive_yourtango = ($exclusive->object_id && !$exclusive->canonical_url) ? 1 : 0;
    //}
    // Meta fields data for the node
    // index | Allow search engines to index this page 
    // follow | Allow search engines to follow links on this page
    // nofollow | Prevent search engines from following links on this page.
    // noarchive | Prevent a cached copy of this page from being available in the search results
    // nosnippet | Prevents a description from appearing below the page in the search results, as well as prevents caching of the page.
    // noodp | Blocks the (link is external) description of the page from being used in the description that appears below the page in the search results. noydir | Prevents Yahoo! from listing this page in the Yahoo! Directory (link is external)
    // noimageindex | Prevent search engines from indexing images on this page.
    // notranslate | Prevent search engines from offering to translation this page in search results.

    $current_row->metatag_title = yt_migration_get_metatag_title($current_row->nid);
    //$metatag_description = yt_migration_get_metatag_data($current_row->nid);
    $metatag_keywords = yt_migration_get_metatag_data($current_row->nid);
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;

    if ($current_row->exclusive_yourtango == 0) {
      $current_row->allow_index = array('noindex' => 'noindex');
    }
    else {
      $current_row->allow_index = array('index' => 'index');
    }

    //Anonymous content migration condition
    if ($current_row->uid == 0) {
      $current_row->uid = 1132;
      $count_words = strip_tags($current_row->body);
      if (count(str_word_count($count_words, 1)) < 200) {
        return FALSE;
      }
    }

    // Image field values pass
    $image = unserialize($current_row->field_image_resource_data);
    $current_row->image_alt = $image['alt'];
    $current_row->image_title = $image['title'];

    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_node_metatag_save($entity, $row);
  }

}

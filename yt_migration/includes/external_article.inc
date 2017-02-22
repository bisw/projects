<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoExternalArticleMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces to External Article');

    $this->dependencies = array('YourTangoTaxonomyLoveStage');
    $this->dependencies = array('YourTangoTaxonomyUserTags');
    $this->dependencies = array('YourTangoTaxonomyCategory');
    $this->dependencies = array('YourTangoTaxonomyBlog');
    $this->dependencies = array('YourTangoTaxonomyFile');
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
        ->fields('n', array('nid', 'vid', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->condition('n.type', 'external_article', '=')
        ->condition('n.uid', '127328', '<=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_external_article', 'ct', 'ct.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_field_image_caption', 'cic', 'cic.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_feature_summar', 'cs', 'cs.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_article_excerp', 'cf', 'cf.vid = n.vid');

    // Join with files
    $query->leftJoin('files', 'file', 'file.fid = ct.field_image_external_article_fid');

    $query->fields('nr', array('body', 'teaser', 'format'));
    $query->fields('file', array('fid', 'filename', 'filepath', 'filemime', 'filesize', 'timestamp'));
    $query->addField('ct', 'field_external_article_link_url');
    $query->addField('ct', 'field_external_article_link_title');
    $query->addField('ct', 'field_external_article_link_attributes');
    $query->addField('ct', 'field_image_external_article_fid');
    $query->addField('ct', 'field_image_external_article_list');
    $query->addField('ct', 'field_image_external_article_data');
    $query->addField('cic', 'field_image_caption_format');
    $query->addField('cic', 'field_image_caption_value');
    $query->addField('cs', 'field_daily_dish_feature_summar_value');
    $query->addField('cf', 'field_daily_dish_article_excerp_value');

    // If there is data from statistic.module, grab the view counts
    if (module_exists('statistics')) {
      $query->join('node_counter', 'nc', 'n.nid = nc.nid');
      $query->addField('nc', 'totalcount');
      $query->addField('nc', 'daycount');
      $query->addField('nc', 'timestamp');
    }
    $query->orderBy('n.nid', 'DESC');

    // Add source fields which not queried in $query, will be populated in prepareRow()
    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'category' => t('The taxonomy category field for the node'),
      'love_stages' => t('The taxonomy love stages  field for the node'),
      'blog' => t('The taxonomy blog field for the node'),
      'user_tags' => t('The taxonomy user tags field for the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'image_filepath' => t('Image path field for the node'),
      'image_alt' => t('Image alt field for the node'),
      'image_title' => t('Image title field for the node'),
      'image_caption' => t('Image caption field for the node'),
      'image_caption_format' => t('Image caption format field for the node'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('partner_link');

    // Mapping: Assign mappings TO destination fields FROM source fields.
    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('nid', 'title', 'uid', 'created', 'changed', 'status', 'promote', 'sticky'));

    // Define default value for some fields
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('revision_uid', 'uid');

    // Mapping of the body field
    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'));
    $this->addFieldMapping('body', 'body')->arguments($body_arguments);
    $this->addFieldMapping('body:format')->defaultValue('full_html');
    $this->addFieldMapping('body:summary', 'field_daily_dish_article_excerp_value')->arguments(array('format' => 'full_html'));

    // Mapping fields not need to be preprocessed (text, user reference to entity reference)
    $this->addFieldMapping('field_website', 'field_external_article_link_url')->defaultValue('');
    $this->addFieldMapping('field_website:title', 'field_external_article_link_title')->defaultValue('');
    $this->addFieldMapping('field_website:attributes', 'field_external_article_link_attributes')->defaultValue('');

    //$this->addFieldMapping('field_block', 'block');
    $this->addFieldMapping('field_image_blog:alt', 'image_alt');
    $this->addFieldMapping('field_image_blog:title', 'image_title');
    $this->addFieldMapping('field_image_blog', 'fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_image_blog:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_image_blog:ignore_case')->defaultValue(TRUE);

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass taxonomy fields values from prepareRow()
    // Pass taxonomy fields values from prepareRow()
    $this->addFieldMapping('field_love_stages', 'love_stages')->sourceMigration('YourTangoTaxonomyLoveStage')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_category', 'category')->sourceMigration('YourTangoTaxonomyCategory')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_blog', 'blog')->sourceMigration('YourTangoTaxonomyBlog')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_user_tags', 'user_tags')->sourceMigration('YourTangoTaxonomyUserTags')->arguments(array('source_type' => 'tid'));
    //$this->addFieldMapping('field_series', 'user_tags')->sourceMigration('YourTangoTaxonomySeries')->arguments(array('source_type' => 'tid'));
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
    //watchdog('Blog Content Type', print_r($current_row, TRUE));
    // Set the term for the node.
    $current_row->love_stages = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_LOVE_STAGES);
    $current_row->category = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_CATEGORY);
    $current_row->blog = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_BLOG);
    $current_row->user_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_USER_TAGS);

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    // Meta fields data for the node
    $current_row->metatag_title = yt_migration_get_metatag_title($current_row->nid);
    $metatag_description = yt_migration_get_metatag_data($current_row->nid, 'description');
    $metatag_keywords = yt_migration_get_metatag_data($current_row->nid, 'keywords');
    $current_row->metatag_description = $metatag_description;
    $current_row->metatag_keywords = $metatag_keywords;

    // Image field values pass
    if (!empty($current_row->field_image_external_article_data)) {
      $image = unserialize($current_row->field_image_external_article_data);
      $current_row->image_filepath = str_replace('sites/default/files/', DRUPAL_ROOT . '/sites/default/files-Prod-15092014/', $current_row->filepath);
      $current_row->image_alt = $image['alt'];
      $current_row->image_title = $image['title'];
    }
    else {
      $current_row->image_filepath = str_replace('sites/default/files/', DRUPAL_ROOT . '/sites/default/files-Prod-15092014/', $current_row->filepath);
      $current_row->image_alt = '';
      $current_row->image_title = '';
    }

    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_image_caption_save($entity, $row);
    yt_migration_node_metatag_save($entity, $row);
  }

}

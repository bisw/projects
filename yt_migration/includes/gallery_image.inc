<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoGalleryImageMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for feature.');
    $this->dependencies = array('YourTangoUsers');
    $this->dependencies = array('YourTangoGallery');
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
        ->condition('n.type', 'node_gallery_image', '=')
        ->condition('n.uid', '127328', '<=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_field_image_caption', 'cic', 'cic.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_node_gallery_image', 'ct', 'ct.vid = n.vid');

    // Join with coauthor field table
    //$query->leftJoin('content_field_coauthor', 'cc', 'cc.vid = n.vid');
    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_feature_summar', 'cs', 'cs.vid = n.vid');

    // Join with paginate field table
    $query->leftJoin('content_field_do_not_paginate', 'cp', 'cp.vid = n.vid');

    // Join with content ad field table
    $query->leftJoin('content_field_do_not_in_content_ad', 'cn', 'cn.vid = n.vid');

    // Join with include in main rss table
    $query->leftJoin('content_field_include_in_main_rss', 'ci', 'ci.vid = n.vid');

    // Join with waterfall field table
    $query->leftJoin('content_field_not_in_waterfall', 'cw', 'cw.vid = n.vid');

    $query->fields('nr', array('body', 'teaser', 'format'));

    //$query->addField('cc', 'field_coauthor_uid');

    $query->addField('cs', 'field_daily_dish_feature_summar_value');

    $query->addField('cp', 'field_do_not_paginate_value');

    $query->addField('cn', 'field_do_not_in_content_ad_value');

    $query->addField('ci', 'field_include_in_main_rss_value');

    $query->addField('cw', 'field_not_in_waterfall_value');

    $query->addField('ct', 'field_node_gallery_image_fid');

    $query->addField('ct', 'field_node_gallery_image_list');

    $query->addField('ct', 'field_node_gallery_image_data');

    $query->addField('ct', 'field_image_alt_value');

    $query->addField('cic', 'field_image_caption_value');

    $query->addField('cic', 'field_image_caption_format');

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
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'gid' => t('Node gallery Reference'),
      'image_fid' => t('File id'),
      'image_alt' => t('Image Alt text'),
      'image_title' => t('Image title'),
      'co_author_uids' => t('Co-Author Uids'),
      'weight' => t('Weight'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('node_gallery_item');

    // Mapping: Assign mappings TO destination fields FROM source fields.
    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('nid', 'title', 'uid', 'created', 'changed', 'status', 'promote', 'sticky'));

    // Define default value for some fields
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('revision')->defaultValue(TRUE);
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('revision_uid', 'uid');

    // Mapping of the body field
    $this->addFieldMapping('field_body', 'body');
    $this->addFieldMapping('field_body:format')->defaultValue('full_html');
    $this->addFieldMapping('body', 'field_image_caption_value');

    $this->addFieldMapping('field_daily_dish_feature_summar', 'field_daily_dish_feature_summar_value')->arguments(array('format' => 'full_html'));
    $this->addFieldMapping('field_coauthor', 'co_author_uids')->sourceMigration('YourTangoUsers');
    $this->addFieldMapping('field_disable_300x100_in_content', 'field_do_not_in_content_ad_value');
    $this->addFieldMapping('field_do_not_paginate', 'field_do_not_paginate_value');
    $this->addFieldMapping('field_include_in_main_rss', 'field_include_in_main_rss_value');
    $this->addFieldMapping('field_not_in_waterfall', 'field_not_in_waterfall_value');
    $this->addFieldMapping('field_alt_text', 'field_image_alt_value');
    $this->addFieldMapping('field_gallery_media', 'image_fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_gallery_media:alt', 'image_alt');
    $this->addFieldMapping('field_gallery_media:title', 'image_title');
    $this->addFieldMapping('field_gallery_media:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldmapping('field_gallery_media:preserve_files')->defaultValue(TRUE);

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');

    $this->addFieldMapping('node_gallery_ref_1', 'gid')->sourceMigration('YourTangoGallery')->arguments(array('source_type' => 'nid'));

    // If there is data from statistic.module, map the view counts
    if (module_exists('statistics')) {
      $this->addSimpleMappings(array('totalcount', 'daycount', 'timestamp'));
    }

    // Unmapped source fields
    $this->addUnmigratedSources(array('tnid', 'translate'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('log', 'tnid', 'translate', 'comment'));
  }

  public function prepareRow($current_row) {

    // Set the text format for the node.
    $current_row->format = yt_migration_get_text_format($current_row->format);

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    //Co-Author	
    $current_row->co_author_uids = yt_migration_get_coauthor($current_row->nid);

    // Meta fields data for the node
    $current_row->metatag_title = yt_migration_get_metatag_title($current_row->nid);
    $metatag_description = yt_migration_get_metatag_data($current_row->nid, 'description');
    $metatag_keywords = yt_migration_get_metatag_data($current_row->nid, 'keywords');
    $current_row->metatag_description = $metatag_description;
    $current_row->metatag_keywords = $metatag_keywords;
    $connection = yt_migration_db_connection();
    $query = $connection->select('node_gallery_images', 'ng')
        ->fields('ng', array('gid', 'weight'))
        ->condition('ng.nid', $current_row->nid, '=');
    $values = $query->execute()->fetchObject();
    $current_row->gid = $values->gid;
    $current_row->weight = $values->weight;
    $unserialize = unserialize($current_row->field_node_gallery_image_data);

    $current_row->image_fid[] = $current_row->field_node_gallery_image_fid;
    $current_row->image_alt[] = $unserialize['alt'];
    $current_row->image_title[] = $unserialize['title'];
    $current_row->image_caption = $current_row->field_image_caption_value;
    $current_row->image_caption_format = $current_row->field_image_caption_format;

    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_image_caption_save($entity, $row, 'field_gallery_media');
    yt_migration_node_metatag_save($entity, $row);
    db_merge('node_gallery_relationship')
        ->key(array(
          'nid' => $row->nid,
          'ngid' => $row->gid,
        ))
        ->fields(array(
          'weight' => $row->weight,
        ))
        ->execute();
  }

}

<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class for media_youtube entities.
 */
class YourTangoEmbbedVideoYoutubeMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migration of Youtube videos');

    $this->map = new MigrateSQLMap($this->machineName, array(
      'field_embedded_media_value' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'description' => 'YouTube URI',
      )
        ), MigrateDestinationMedia::getKeySchema()
    );

    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('nid', 'uid'))
        ->condition('n.type', 'embedded_video', '=')
        ->condition('ct.field_embedded_media_provider', 'youtube', '=');

    // Grab other fields which can mapping directly
    $query->join('content_type_embedded_video', 'ct', 'ct.vid = n.vid');

    $query->addField('ct', 'field_embedded_media_embed');
    $query->addField('ct', 'field_embedded_media_value');
    $query->addField('ct', 'field_embedded_media_provider');
    $query->addField('ct', 'field_embedded_media_data');
    $query->addField('ct', 'field_embedded_media_version');
    $query->addField('ct', 'field_embedded_media_duration');

    $source_fields = array(
      'uri' => t('URI of a YouTube video'),
      'description' => t('Description of the video'),
    );

    $this->source = new MigrateSourceSQL($query, $source_fields);

    // In this case, we need to specify the file_class in the second paramter -
    // this class understands how to translate a http://www.youtube.com/ URI
    // into Drupal's Youtube file scheme (youtube://).
    $this->destination = new MigrateDestinationMedia('video', 'MigrateExtrasFileYoutube');

    // We just need to map the 'value' in the media destination to the Youtube
    // URI.
    $this->addFieldMapping('value', 'uri');

    $this->addFieldMapping('field_video_description', 'description');
    $this->addFieldMapping('uid', 'uid');
  }

  public function prepareRow($current_row) {
//  print_r($current_row);die;
    $current_row->uri = "https://www.youtube.com/watch?v=" . $current_row->field_embedded_media_value . "&feature=youtube_gdata_player";
    $current_row->field_embedded_media_embed = "https://www.youtube.com/watch?v=" . $current_row->field_embedded_media_value . "&feature=youtube_gdata_player";
  }

}

/**
 * Migration class for media_youtube entities.
 */
class YourTangoEmbbedVideoBlipTvMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migration of Blip TV videos');

    $this->map = new MigrateSQLMap($this->machineName, array(
      'field_embedded_media_embed' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'description' => 'BlipTV URI',
      )
        ), MigrateDestinationMedia::getKeySchema()
    );

    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('nid', 'uid'))
        ->condition('n.type', 'embedded_video', '=')
        ->condition('ct.field_embedded_media_provider', 'bliptv', '=');

    // Grab other fields which can mapping directly
    $query->join('content_type_embedded_video', 'ct', 'ct.vid = n.vid');

    $query->addField('ct', 'field_embedded_media_embed');
    $query->addField('ct', 'field_embedded_media_value');
    $query->addField('ct', 'field_embedded_media_provider');
    $query->addField('ct', 'field_embedded_media_data');
    $query->addField('ct', 'field_embedded_media_version');
    $query->addField('ct', 'field_embedded_media_duration');

    $source_fields = array(
      'field_embedded_media_embed' => t('URI of a YouTube video'),
      'description' => t('Description of the video'),
    );

    $this->source = new MigrateSourceSQL($query, $source_fields);

    // In this case, we need to specify the file_class in the second paramter -
    // this class understands how to translate a http://www.youtube.com/ URI
    // into Drupal's Youtube file scheme (youtube://).
    $this->destination = new MigrateDestinationMedia('video', 'MigrateExtrasFileBlipTv');

    // We just need to map the 'value' in the media destination to the Youtube
    // URI.
    $this->addFieldMapping('value', 'field_embedded_media_embed');

    $this->addFieldMapping('field_video_description', 'description');
    $this->addFieldMapping('uid', 'uid');
  }

  public function prepareRow($current_row) {
    
  }

}

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoEmbbedVideoMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for feature.');
    /* $this->dependencies = array(
      'YourTangoTaxonomyLoveStage',
      'YourTangoTaxonomyUserTags',
      'YourTangoTaxonomyCategory',
      'YourTangoTaxonomyBlog',
      'YourTangoTaxonomyFile',
      'YourTangoUsers'
      ); */

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
        ->condition('n.type', 'embedded_video', '=')
        ->condition('n.uid', '127328', '<=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_embedded_video', 'ct', 'ct.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_field_embedded_video', 'cv', 'cv.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_field_image_caption', 'cic', 'cic.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_feature_summar', 'cs', 'cs.vid = n.vid');

    // Join with extra content field table
    $query->leftJoin('content_field_list_extra_content', 'ce', 'ce.vid = n.vid');

    // Join with include in main rss table
    $query->leftJoin('content_field_include_in_main_rss', 'ci', 'ci.vid = n.vid');

    // Join with paginate field table
    $query->leftJoin('content_field_do_not_paginate', 'cp', 'cp.vid = n.vid');

    //
    $query->leftJoin('content_field_embedded_video_image', 'cfv', 'cfv.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_article_excerp', 'cf', 'cf.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_do_not_in_content_ad', 'cfa', 'cfa.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_approved', 'cfap', 'cfap.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_google_news_meta_tags', 'cgm', 'cgm.vid = n.vid');

    // Tango Media
    $query->leftJoin('tango_media_node', 'tm', 'tm.vid = n.vid');

    $query->fields('nr', array('body', 'teaser', 'format'));

    $query->addField('ct', 'field_embedded_media_embed');
    $query->addField('ct', 'field_embedded_media_value');
    $query->addField('ct', 'field_embedded_media_provider');
    $query->addField('ct', 'field_embedded_media_data');
    $query->addField('ct', 'field_embedded_media_version');
    $query->addField('ct', 'field_embedded_media_duration');
    $query->addField('ct', 'field_image_embedded_video_fid');
    $query->addField('ct', 'field_image_embedded_video_list');
    $query->addField('ct', 'field_image_embedded_video_data');
    $query->addField('ct', 'field_embedded_video_one_screen_value');
    $query->addField('ct', 'field_embedded_video_video_frame_fid');
    $query->addField('ct', 'field_embedded_video_video_frame_list');
    $query->addField('ct', 'field_embedded_video_video_frame_data');
    $query->addField('ct', 'field_embedded_ndn_video_id_value');
    $query->addField('cs', 'field_daily_dish_feature_summar_value');
    $query->addField('cf', 'field_daily_dish_article_excerp_value');
    $query->addField('cic', 'field_image_caption_value');
    $query->addField('cv', 'field_embedded_video_embed');
    $query->addField('cv', 'field_embedded_video_value');
    $query->addField('cv', 'field_embedded_video_provider');
    $query->addField('cv', 'field_embedded_video_version');
    $query->addField('cv', 'field_embedded_video_data');
    $query->addField('cv', 'field_embedded_video_duration');
    $query->addField('ce', 'field_list_extra_content_value');
    $query->addField('ci', 'field_include_in_main_rss_value');

    $query->addField('cp', 'field_do_not_paginate_value');
    $query->addField('cfa', 'field_do_not_in_content_ad_value');

    $query->addField('cfv', 'field_embedded_video_image_fid');
    $query->addField('cfv', 'field_embedded_video_image_list');
    $query->addField('cfv', 'field_embedded_video_image_data');
    $query->addField('cfap', 'field_approved_value');
    $query->addField('cgm', 'field_google_news_meta_tags_value');
    $query->addField('tm', 'sitemap');

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
      'series' => t('The taxonomy series field for the node'),
      'super_duper_tags' => t('The taxonomy Super  Duper Tags field for the node'),
      'super_tags' => t('The taxonomy Super Tags field for the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'image_filepath' => t('Image path field for the node'),
      'image_alt' => t('Image alt field for the node'),
      'image_title' => t('Image title field for the node'),
      'image_caption' => t('Image caption field for the node'),
      'image_caption_format' => t('Image caption format field for the node'),
      'exclusive_yourtango' => t('Exclusive to your tango'),
      'field_meta_canonical_url' => t('External site link'),
      'allow_index' => t('Allow index'),
      'field_website_name' => t('Website Name'),
      'field_embedded_ndn_video_id_value_integer' => t('Video ID'),
      'co_author_uids' => t('Co-Author Uids'),
      'video_uri' => t('Embedded Media URLs'),
      'video_fid' => t('Embbed Video Fids'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('embedded_video');

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
    $this->addFieldMapping('body:summary', 'field_daily_dish_article_excerp_value');

    // Mapping fields not need to be preprocessed (text, user reference to entity reference)
    $this->addFieldMapping('field_list_extra_content', 'field_list_extra_content_value');
    $this->addFieldMapping('field_list_extra_content:format')->defaultValue('full_html');
    $this->addFieldMapping('field_coauthor', 'co_author_uids')->sourceMigration('YourTangoUsers');
    $this->addFieldMapping('field_include_in_main_rss', 'field_include_in_main_rss_value');
    $this->addFieldMapping('field_do_not_paginate', 'field_do_not_paginate_value');
    $this->addFieldMapping('field_daily_dish_feature_summar', 'field_daily_dish_feature_summar_value');
    $this->addFieldMapping('field_meta_exclusive_content', 'exclusive_yourtango');
    $this->addFieldMapping('field_meta_canonical_url', 'field_meta_canonical_url');
    $this->addFieldMapping('field_disable_300x100_in_content', 'field_do_not_in_content_ad_value');
    $this->addFieldMapping('field_meta_website_name', 'field_website_name');
    $this->addFieldMapping('field_include_in_news_sitemap', 'sitemap')->defaultValue('0');

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass taxonomy fields values from prepareRow()
    $this->addFieldMapping('field_love_stages', 'love_stages')->sourceMigration('YourTangoTaxonomyLoveStage')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_category', 'category')->sourceMigration('YourTangoTaxonomyCategory')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_blog', 'blog')->sourceMigration('YourTangoTaxonomyBlog')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_user_tags', 'user_tags')->sourceMigration('YourTangoTaxonomyUserTags')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_series', 'series')->sourceMigration('YourTangoTaxonomySeries')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_super_duper_tags', 'super_duper_tags')->sourceMigration('YourTangoTaxonomySuperDuperTags')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_super_tags', 'super_tags')->sourceMigration('YourTangoTaxonomySuperTags')->arguments(array('source_type' => 'tid'));

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_robots', 'allow_index');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');
    $this->addFieldMapping('metatag_news_keywords', 'field_google_news_meta_tags_value');
    $this->addFieldMapping('field_page_title', 'metatag_title');

    // Embedded Media 	
    $this->addFieldMapping('field_embedded_media:display')->defaultValue(TRUE);

    $this->addFieldMapping('field_image_blog:alt', 'image_alt');
    $this->addFieldMapping('field_image_blog:title', 'image_title');
    $this->addFieldMapping('field_image_blog', 'field_image_embedded_video_fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_image_blog:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldmapping('field_image_blog:preserve_files')->defaultValue(TRUE);

    $this->addFieldMapping('field_video_frame', 'field_embedded_video_video_frame_fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_video_frame:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldmapping('field_video_frame:preserve_files')->defaultValue(TRUE);

    $this->addFieldMapping('field_embedded_ndn_video_id', 'field_embedded_ndn_video_id_value_integer');

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

    // Set the term for the node.
    $current_row->love_stages = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_LOVE_STAGES);
    $current_row->category = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_CATEGORY);
    $current_row->blog = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_BLOG);
    $current_row->user_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_USER_TAGS);
    $current_row->series = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_SERIES);
    $current_row->super_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_SUPER_TAGS);
    $current_row->super_duper_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_SUPER_DUPER_TAGS);

    //Co-Author	
    $current_row->co_author_uids = yt_migration_get_coauthor($current_row->nid);

    //Media Migration
    if ($current_row->field_embedded_media_provider == 'youtube') {
      $current_row->video_fid = yt_migration_get_video_fid($current_row->field_embedded_media_value, 'youtube');
      $current_row->video_uri = "http://www.youtube.com/watch?v=" . $current_row->field_embedded_media_value . "&feature=youtube_gdata_player";
    }
    else if ($current_row->field_embedded_media_provider == 'bliptv') {
      $current_row->video_fid = yt_migration_get_video_fid($current_row->field_embedded_media_embed, 'bliptv');
      $current_row->video_uri = $current_row->field_embedded_media_embed;
    }
    else {
      $current_row->video_fid = '';
      $current_row->video_uri = '';
    }

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    /** Exclusive to your tango Logic* */
    $exclusive = yt_migration_tango_exclusive($current_row->nid);

    $current_row->exclusive_yourtango = ($exclusive->object_id && !$exclusive->canonical_url) ? 1 : 0;

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
    $metatag_keywords = yt_migration_get_metatag_data($current_row->nid);
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $exclusive->canonical_url;
    $current_row->field_website_name = $exclusive->website_name;

    // When “Exclusive” to YourTango” field is “Yes” & the content is “approved”. 	- 	Index/follow
    // When “Exclusive” to YourTango” field is “Yes” & the content is “not approved”. 	-	No index/No follow
    // When “Exclusive” to YourTango” field is “NO” & the content is “approved”.	- 	Index/no follow
    // When “Exclusive” to YourTango” field is “No” & the content is “not approved”.	-	No index/no follow

    if ($current_row->exclusive_yourtango == 1 && $current_row->field_approved_value == 1) {
      $current_row->allow_index = array('index' => 'index', 'follow' => 'follow');
    }
    else if ($current_row->exclusive_yourtango == 1 && $current_row->field_approved_value == 0) {
      $current_row->allow_index = array('noindex' => 'noindex', 'nofollow' => 'nofollow');
    }
    else if ($current_row->exclusive_yourtango == 0 && $current_row->field_approved_value == 1) {
      $current_row->allow_index = array('index' => 'index', 'nofollow' => 'nofollow');
    }
    else if ($current_row->exclusive_yourtango == 0 && $current_row->field_approved_value == 0) {
      $current_row->allow_index = array('noindex' => 'noindex', 'nofollow' => 'nofollow');
    }

    $image = unserialize($current_row->field_image_embedded_video_data);
    $current_row->image_alt = $image['alt'];
    $current_row->image_title = $image['title'];
    $current_row->image_caption = $current_row->field_image_caption_value;
    $current_row->image_caption_format = $current_row->field_image_caption_format;

    if (is_numeric($current_row->field_embedded_ndn_video_id_value)) {
      $current_row->field_embedded_ndn_video_id_value_integer = $current_row->field_embedded_ndn_video_id_value;
    }
    else {
      $current_row->field_embedded_ndn_video_id_value_integer = '';
    }

    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_image_caption_save($entity, $row);
    yt_migration_node_metatag_save($entity, $row);
    if ($row->video_fid > 0) {

      db_merge('field_data_field_embedded_media')
          ->fields(
              array(
                'field_embedded_media_fid' => $row->video_fid,
                'field_embedded_media_display' => 1,
                'field_embedded_media_description' => '',
                'deleted' => '0',
          ))
          ->key(array(
            'delta' => 0,
            'entity_id' => $entity->nid,
            'entity_type' => 'node',
            'bundle' => 'embedded_video',
            'revision_id' => $entity->vid,
            'language' => LANGUAGE_NONE
          ))
          ->execute();

      db_merge('field_revision_field_embedded_media')
          ->fields(
              array(
                'field_embedded_media_fid' => $row->video_fid,
                'field_embedded_media_display' => 1,
                'field_embedded_media_description' => '',
                'deleted' => '0'
          ))
          ->key(array(
            'delta' => 0,
            'entity_id' => $entity->nid,
            'entity_type' => 'node',
            'bundle' => 'embedded_video',
            'revision_id' => $entity->vid,
            'language' => LANGUAGE_NONE
          ))
          ->execute();
    }
  }

}

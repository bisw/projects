<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoGalleryMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for gallery.');

    $this->dependencies = array('YourTangoTaxonomyLoveStage');
    $this->dependencies = array('YourTangoTaxonomyUserTags');
    $this->dependencies = array('YourTangoTaxonomyCategory');
    $this->dependencies = array('YourTangoTaxonomyBlog');
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
        ->condition('n.type', 'node_gallery_gallery', '=')
        ->condition('n.uid', '127328', '<=');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_node_gallery_gallery', 'ct', 'ct.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_field_image_caption', 'cic', 'cic.vid = n.vid');

    // Join with coauthor field table
    //$query->leftJoin('content_field_coauthor', 'cc', 'cc.vid = n.vid');
    // Join with author field table
    $query->leftJoin('content_field_author', 'ca', 'ca.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_daily_dish_feature_summar', 'cs', 'cs.vid = n.vid');

    // Join with include in main rss table
    $query->leftJoin('content_field_include_in_main_rss', 'ci', 'ci.vid = n.vid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_google_news_meta_tags', 'cgm', 'cgm.vid = n.vid');

    // Join with files
    $query->leftJoin('files', 'file', 'file.fid = ct.field_image_node_gallery_gallery_fid');

    // Join with disable node thumbnail field table
    $query->leftJoin('content_field_approved', 'cfa', 'cfa.vid = n.vid');

    // Tango Media
    $query->leftJoin('tango_media_node', 'tm', 'tm.vid = n.vid');

    $query->fields('nr', array('body', 'teaser', 'format'));
    $query->fields('file', array('fid', 'filename', 'filepath', 'filemime', 'filesize', 'timestamp'));
    $query->addField('ct', 'field_image_node_gallery_gallery_fid');
    $query->addField('ct', 'field_image_node_gallery_gallery_list');
    $query->addField('ct', 'field_image_node_gallery_gallery_data');
    $query->addField('ca', 'field_author_uid');
    $query->addField('cs', 'field_daily_dish_feature_summar_value');
    $query->addField('cic', 'field_image_caption_value');
    $query->addField('cic', 'field_image_caption_format');
    //$query->addField('cc', 'field_coauthor_uid');
    $query->addField('ci', 'field_include_in_main_rss_value');
    $query->addField('cfa', 'field_approved_value');
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
      'co_author_uids' => t('Co-Author Uids')
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('node_gallery_gallery');

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
    $this->addFieldMapping('field_coauthor', 'co_author_uids')->sourceMigration('YourTangoUsers');
    $this->addFieldMapping('field_include_in_main_rss', 'field_include_in_main_rss_value');
    $this->addFieldMapping('field_include_in_news_sitemap', 'sitemap')->defaultValue('0');

    // Pass path alias values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Pass taxonomy fields values from prepareRow()
    $this->addFieldMapping('field_love_stages', 'love_stages')->sourceMigration('YourTangoTaxonomyLoveStage')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_category', 'category')->sourceMigration('YourTangoTaxonomyCategory')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_blog', 'blog')->sourceMigration('YourTangoTaxonomyBlog')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_user_tags', 'user_tags')->sourceMigration('YourTangoTaxonomyUserTags')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_super_duper_tags', 'super_duper_tags')->sourceMigration('YourTangoTaxonomySuperDuperTags')->arguments(array('source_type' => 'tid'));
    $this->addFieldMapping('field_super_tags', 'super_tags')->sourceMigration('YourTangoTaxonomySuperTags')->arguments(array('source_type' => 'tid'));

    // Pass metadata fields values from prepareRow()
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('field_page_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_robots', 'allow_index');
    $this->addFieldMapping('field_meta_exclusive_content', 'exclusive_yourtango');
    $this->addFieldMapping('field_meta_canonical_url', 'field_meta_canonical_url');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');
    $this->addFieldMapping('field_meta_website_name', 'field_website_name');
    $this->addFieldMapping('metatag_news_keywords', 'field_google_news_meta_tags_value');

    // Mapping file fields
    $this->addFieldMapping('field_image_blog:alt', 'image_alt');
    $this->addFieldMapping('field_image_blog:title', 'image_title');
    $this->addFieldMapping('field_image_blog', 'fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_image_blog:file_class')->defaultValue('MigrateFileFid');
    //$this->addFieldMapping('field_image_blog:ignore_case')->defaultValue(TRUE);
    //image_field_caption

    $this->addFieldMapping('field_daily_dish_feature_summar', 'field_daily_dish_feature_summar_value');

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
    //watchdog('Blog Content Type', print_r($current_row, TRUE));
    // Set the term for the node.
    $current_row->love_stages = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_LOVE_STAGES);
    $current_row->category = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_CATEGORY);
    $current_row->blog = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_BLOG);
    $current_row->user_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_USER_TAGS);
    $current_row->super_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_SUPER_TAGS);
    $current_row->super_duper_tags = yt_migration_get_terms($current_row->vid, SOURCE_VOCABULAY_SUPER_DUPER_TAGS);

    // Set the url alias for the node.
    $current_row->url_alias = yt_migration_get_url_alias($current_row->nid);

    //Co-Author	
    $current_row->co_author_uids = yt_migration_get_coauthor($current_row->nid);

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

    // Image field values pass
    if (!empty($current_row->field_image_node_gallery_gallery_data)) {
      $image = unserialize($current_row->field_image_node_gallery_gallery_data);
      //$current_row->image_filepath = str_replace('sites/default/files/', DRUPAL_ROOT . '/sites/default/files-Prod-15092014/', $current_row->filepath);
      $current_row->image_alt = $image['alt'];
      $current_row->image_title = $image['title'];
      $current_row->image_caption = isset($current_row->field_image_caption_value) ? $current_row->field_image_caption_value : NULL;
      $current_row->image_caption_format = $current_row->field_image_caption_format;
    }
    else {
      //$current_row->image_filepath = str_replace('sites/default/files/', DRUPAL_ROOT . '/sites/default/files-Prod-15092014/', $current_row->filepath);
      $current_row->image_alt = '';
      $current_row->image_title = '';
      $current_row->image_caption = NULL;
      $current_row->image_caption_format = NULL;
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

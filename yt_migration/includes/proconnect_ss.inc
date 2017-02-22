<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';
require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'field_collection') . '/field_collection.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoProConnectSSMigration extends YourTangoMigration {

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
        ), MigrateDestinationFieldCollection::getKeySchema()
    );

    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('nid', 'vid', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->fields('ct', array('field_client_type_value'))
        ->fields('ci', array('field_embedded_video_image_fid', 'field_embedded_video_image_list', 'field_embedded_video_image_data'))
        ->fields('cv', array('field_embedded_video_embed', 'field_embedded_video_value', 'field_embedded_video_provider', 'field_embedded_video_data', 'field_embedded_video_version', 'field_embedded_video_duration'))
        ->fields('file', array('fid', 'filename', 'filepath', 'filemime', 'filesize', 'timestamp'))
        ->condition('n.type', 'proconnect_success_story', '=')
        ->condition('n.uid', '0', '<>');

    $query->fields('nr', array('body', 'teaser', 'format'));

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_proconnect_success_story', 'ct', 'ct.vid = n.vid');

    // Grab embbed video fields which can mapping directly
    $query->leftJoin('content_field_embedded_video', 'cv', 'cv.vid = n.vid');

    // Grab embbed video image fields which can mapping directly
    $query->leftJoin('content_field_embedded_video_image', 'ci', 'ci.vid = n.vid');

    // Join with files
    $query->leftJoin('files', 'file', 'file.fid = ci.field_embedded_video_image_fid');

    $query->orderBy('n.nid', 'DESC');

    // Add source fields which not queried in $query, will be populated in prepareRow()
    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'image_filepath' => t('Image path field for the node'),
      'image_alt' => t('Image alt field for the node'),
      'image_title' => t('Image title field for the node'),
      'image_caption' => t('Image caption field for the node'),
      'image_caption_format' => t('Image caption format field for the node'),
      'pid' => t('Profile ID'),
      'video_uri' => t('Embedded Media URLs'),
      'video_fid' => t('Embbed Video Fids'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationFieldCollection(
        'field_success_stories', array(
      'host_entity_type' => 'profile2'
        )
    );

    $this->addFieldMapping('host_entity_id', 'uid')->sourceMigration('YourTangoAuthor');

    $body_arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'teaser'));
    $this->addFieldMapping('field_success_body', 'body')->arguments($body_arguments);
    $this->addFieldMapping('field_success_body:format')->defaultValue('full_html');
    $this->addFieldMapping('field_title', 'title');
    $this->addFieldMapping('field_type_of_client', 'field_client_type_value');

    // Mapping file fields
    $this->addFieldMapping('field_image_blog:alt', 'image_alt');
    $this->addFieldMapping('field_image_blog:title', 'image_title');
    $this->addFieldMapping('field_image_blog', 'fid')->sourceMigration('YourTangoFile')->arguments(array('source_type' => 'fid'));
    $this->addFieldMapping('field_image_blog:file_class')->defaultValue('MigrateFileFid');
    $this->addFieldMapping('field_image_blog:preserve_files')->defaultValue('1');
  }

  public function prepareRow($current_row) {
    // Set the text format for the node.
    $current_row->format = yt_migration_get_text_format($current_row->format);
    $query = db_select('profile', 'p')
        ->fields('p', array('pid'))
        ->condition('p.uid', $current_row->uid, '=');
    $values = $query->execute()->fetchObject();
    $current_row->pid = $values->pid;

    // Image field values pass
    if (!empty($current_row->field_embedded_video_image_data)) {
      $image = unserialize($current_row->field_embedded_video_image_data);
      $current_row->image_alt = $image['alt'];
      $current_row->image_title = $image['title'];
      $current_row->image_caption = $current_row->field_image_caption_value;
      $current_row->image_caption_format = $current_row->field_image_caption_format;
    }
    else {
      $current_row->image_alt = '';
      $current_row->image_title = '';
      $current_row->image_caption = '';
      $current_row->image_caption_format = '';
    }

    //Media Migration
    if ($current_row->field_embedded_video_provider == 'youtube') {
      $current_row->video_fid = yt_migration_get_video_fid("http://www.youtube.com/watch?v=" . $current_row->field_embedded_video_value . "&feature=youtube_gdata_player", 'youtube');
      $current_row->video_uri = "http://www.youtube.com/watch?v=" . $current_row->field_embedded_video_value . "&feature=youtube_gdata_player";
    }
    else if ($current_row->field_embedded_video_provider == 'bliptv') {
      $current_row->video_fid = yt_migration_get_video_fid($current_row->field_embedded_video_embed, 'bliptv');
      $current_row->video_uri = $current_row->field_embedded_video_embed;
    }
    else {
      $current_row->video_fid = '';
      $current_row->video_uri = '';
    }


    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    if ($row->video_fid > 0) {
      $count_query = db_select('field_data_field_embedded_media', 'fd');
      $count_query->fields('fd', array('entity_id'));
      $count_query->condition('fd.entity_id', $entity->pid);
      $count_query->condition('fd.entity_type', 'field_collection_item');
      $count_query->condition('fd.bundle', 'field_success_stories');
      $count_query->condition('fd.revision_id', $entity->pid);
      $count = count($count_query->execute());
      db_merge('field_data_field_embedded_media')
          ->fields(
              array(
                'field_embedded_media_fid' => $row->video_fid,
                'field_embedded_media_display' => 1,
                'field_embedded_media_description' => '',
                'deleted' => '0',
          ))
          ->key(array(
            'delta' => $count,
            'entity_id' => $entity->pid,
            'entity_type' => 'field_collection_item',
            'bundle' => 'field_success_stories',
            'revision_id' => $entity->pid,
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
            'delta' => $count,
            'entity_id' => $entity->pid,
            'entity_type' => 'field_collection_item',
            'bundle' => 'field_success_stories',
            'revision_id' => $entity->pid,
            'language' => LANGUAGE_NONE
          ))
          ->execute();
    }
  }

}

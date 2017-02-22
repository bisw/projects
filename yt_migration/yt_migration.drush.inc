<?php

/**
 * Implementation of hook_drush_command().
 */
function yt_migration_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['yt-mig-latlon'] = array(
    'callback' => 'yt_migration_drush_latlon',
    'description' => "Run migration script for proximity search after author migration.",
  );
  $items['yt-canonical-field-update'] = array(
    'callback' => 'yt_canonical_field_update',
    'description' => "Run migration to update canonical field value according to own url.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-canonical-field-update --limit=500 --start=0' => 'Run script to update canonical field value according to own url.',
    ),
  );
  $items['yt-update-experts'] = array(
    'callback' => 'yt_migration_update_experts',
    'description' => "Change migration status to update experts.",
  );
  $items['yt-mig-ref-image'] = array(
    'callback' => 'yt_migration_drush_ref_image',
    'description' => "Run migration script for migrating reference images in blog.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-mig-ref-image --limit=500 --start=0' => 'Run script migrating reference image.',
    ),
  );
  $items['yt-published-latest-content'] = array(
    'callback' => 'yt_published_latest_content',
    'description' => "Run migration script for published latest node revison.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-published-latest-content --limit=500 --start=0' => 'Run migration script for published latest node revison.',
    ),
  );
  $items['yt-user-alias'] = array(
    'callback' => 'yt_migration_drush_user_alias',
    'description' => "Run script for migrating users alias except user 1.",
  );
  $items['yt-update-content-aliases'] = array(
    'callback' => 'yt_migration_update_content_alias',
    'description' => "Run script for updating the content aliases.",
    'aliases' => array('yuca'),
  );
  $items['yt-old-url-redirect'] = array(
    'callback' => 'yt_old_url_redirect',
    'description' => "Run script for getting old url of the content.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),    
  );
  $items['yt-pending-users-list'] = array(
    'callback' => 'yt_pending_users_list',
    'description' => "Run script for getting user list which are not created through script.",
    'options' => array(
    	'uid' => dt('Last migrated uid of the users'),
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),    
  );   
  $items['yt-delete-ss'] = array(
    'callback' => 'yt_migration_drush_delete_ss',
    'description' => "Run script for deleting blank success stories.",
  );
  $items['yt-sanitize-d6-db'] = array(
    'callback' => 'yt_migration_sanitize_d6_db',
    'description' => "Run script for sanitized drupal 6 db.",
  );
  $items['yt-reprisal-date'] = array(
    'callback' => 'yt_migration_reprisal_date_update',
    'description' => "Run script for updating the old content reprisal date.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-reprisal-date --limit=500 --start=0' => 'Update all blog content last 24 hours.(1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
    ),
  );
  $items['yt-node-revisions-delete'] = array(
    'callback' => 'yt_migration_delete_all_revisions',
    'description' => "Run script for deleting old revisions of the node.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-node-revisions-delete --limit=500 --start=0' => 'Delete all old revisions of the content.',
    ),
  );
  $items['yt-revert-userinfo'] = array(
    'callback' => 'yt_migration_revert_userinfo',
    'description' => "Run script reverting user info (name, mail and init).",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-revert-userinfo --limit=500 --start=0' => 'Run script reverting user info (name, mail and init).',
    ),
  );
  $items['yt-redirect-delete'] = array(
    'callback' => 'yt_redirect_unused_delete',
    'description' => "Run script to delete unused redirect.",
  );
  /* $items['yt-update-user-roles'] = array(
    'callback' => 'yt_update_users_roles',
    'description' => "Run script to delete unused redirect.",
    ); */
  $items['yt-update-user-join-date'] = array(
    'callback' => 'yt_update_users_join_date',
    'description' => "Run script to update user join dates.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-update-user-join-date --limit=500 --start=0' => 'Update all users join dates',
    ),
  );
  $items['yt-update-user-roles'] = array(
    'callback' => 'yt_update_users_roles',
    'description' => "Run script to update user roles.",
  );
  /*  $items['yt-update-node-credit-field'] = array(
    'callback' => 'yt_update_node_credit_field',
    'description' => "Run script to update credit field.",
    'options' => array(
    'limit' => dt('Set the limit of ids'),
    'start' => dt('Start records from like 100')
    ),
    'examples' => array(
    'yt-update-node-credit-field --limit=500 --start=0' => 'Update all credit field',
    ),
    );*
   */
  $items['yt-files-migration'] = array(
    'callback' => 'yt_migration_files_migration',
    'description' => "Run script to migrate the files from live site.",
    'options' => array(
      'fid' => dt('Particluar file ID'),
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-files-migration --limit=500 --start=0' => 'Update all users join dates',
      'yt-files-migration --fid=xxx' => 'Update all users join dates',
    ),
  );
  $items['yt-files-d7-migration'] = array(
    'callback' => 'yt_migration_d7_files_migration',
    'description' => "Run script to migrate the files from live site.",
    'options' => array(
      'fid' => dt('Particluar file ID'),
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-files-d7-migration --limit=500 --start=0' => 'Update all users join dates',
      'yt-files-d7-migration --fid=xxx' => 'Update all users join dates',
    ),
  );  
  $items['yt-picture-migration'] = array(
    'callback' => 'yt_migration_picture_migration',
    'description' => "Run script to migrate the user picture from live site.",
  );
//  $items['yt-experts-vanity'] = array(
//    'callback' => 'yt_migration_experts_vanity',
//    'description' => "Run script for updating meta tags.",
//  );  
  $items['yt-meta-tags-all'] = array(
    'callback' => 'yt_migration_drush_matatags',
    'description' => "Run script for updating meta tags.",
    'options' => array(
      'entity' => dt('Pass entity type node or taxonomy'),
      'type' => dt('Pass type of content'),
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),
    'examples' => array(
      'yt-meta-tags-all --entity=node --type=blog --limit=500 --start=0' => 'Update all meta content meta tags',
      'yt-meta-tags-all --entity=taxonomy --limit=500 --start=0' => 'Update all meta content meta tags',
    ),
  );
  $items['yt-meta-tags-node-type'] = array(
    'callback' => 'yt_migration_drush_matatags_node_types',
    'description' => "Run script for updating list, gallery and embbed videos meta tags.",
    'examples' => array(
      'yt-meta-tags-node-type' => 'Update all list, gallery and embbed videos meta content meta tags',
    ),
  );
  $items['yt-node-field-updates'] = array(
    'callback' => 'yt_node_fields_updates',
    'description' => "Run script to updating some of profile fields.",
  );
  $items['yt-delete-duplicate-aliases'] = array(
    'callback' => 'yt_delete_duplicate_aliases',
    'description' => "Run script to delete the duplicate alias of the gallery items.",
    'options' => array(
      'limit' => dt('Set the limit of ids'),
      'start' => dt('Start records from like 100')
    ),    
  );  
  $items['yt-delete-d7'] = array(
    'callback' => 'yt_migration_drush_delete_content',
    'description' => "Run script for deleting the content new ids created after migration.",
    'arguments' => array(
      'type' => 'Type of the taking updates. Delimit multiple using commas.',
    ),
    'options' => array(
      'timestamp' => dt('Set a time based updates'),
      'id' => dt('Select a last migrate id of content/user'),
    ),
    'examples' => array(
      'yt-delete-d7 node --timestamp=86400 --id=xxxx' => 'Update all blog content last 24 hours.(1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
      'yt-delete-d7 taxonomy --id=xxxx' => 'Update all categories.',
      'yt-delete-d7 user --id=xxxx' => 'Update all user access last 24 hours. Options (1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
    ),
    'aliases' => array('ytdu'),
  );
  $items['yt-update-changes'] = array(
    'callback' => 'yt_migration_drush_update_changes',
    'description' => "Run script for check update content.",
    'arguments' => array(
      'type' => 'Type of the taking updates. Delimit multiple using commas.',
    ),
    'options' => array(
      'timestamp' => dt('Set a time based updates'),
      'id' => dt('Select a last migrate id of content/user'),
    ),
    'examples' => array(
      'yt-update-changes node --timestamp=86400 --id=xxxx' => 'Update all blog content last 24 hours.(1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
      'yt-update-changes updatednodes --timestamp=86400' => 'Update all blog content last 24 hours.(1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
      'yt-update-changes taxonomy' => 'Update all categories.',
      'yt-update-changes user --timestamp=86400 --id=xxxx' => 'Update all user access last 24 hours. Options (1 Month: 2592000, 1 Week: 604800, 1 Hour: 3600)',
    ),
    'aliases' => array('ytuc'),
  );
  return $items;
}

/**
 * Run for updated blog in migration
 */
function yt_update_article_content() {
  $connection = yt_migration_db_connection();
  $query = $connection->select('node', 'n')->fields('n', array('nid', 'uid'));
  $query->orderBy('n.changed', 'DESC');
  $result = $query->execute()->fetchAll();
  foreach ($result as $id => $value) {
  	
  }
}

function yt_published_latest_content() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $nodes = db_select('node', 'n')->fields('n', array('nid', 'vid', 'type'));
    $nodes->range($start, $limit);
    $nodes->orderBy('n.created', 'DESC');
    $node_results = $nodes->execute()->fetchAll();
    foreach ($node_results as $node) {
      $revisions = db_select('node_revision', 'n')
          ->fields('n', array('vid'))
          ->condition('n.nid', $node->nid)
          ->orderBy('n.vid', 'DESC')
          ->range(0, 1)
          ->execute();
      foreach ($revisions as $rev) {
        db_update('node')
            ->fields(array(
              'vid' => $rev->vid
            ))
            ->condition('nid', $node->nid, '=')
            ->execute();
        if ($rev->vid != $revisions->vid) {
          $fields = db_select('field_config_instance', 'f')
              ->fields('f', array('field_name'))
              ->condition('f.bundle', $node->type)
              ->condition('f.entity_type', 'node')
              ->condition('f.deleted', '0')
              ->execute();
          foreach ($fields as $field) {
            $table_name = 'field_data_' . $field->field_name;
            if (db_table_exists($table_name)) {
              db_update($table_name)
                  ->fields(array(
                    'revision_id' => $rev->vid
                  ))
                  ->condition('entity_id', $node->nid, '=')
                  ->execute();
            }
          }
        }
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * 
 */
function yt_canonical_field_update() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $query = db_select('field_data_field_meta_canonical_url', 'fc')->fields('fc');
    $query->range($start, $limit);
    $query->orderBy('fc.entity_id', 'ASC');
    $query->condition('fc.entity_type', 'node', '=');
    $canonicals = $query->execute();
    foreach ($canonicals as $id => $canonical) {
      drush_print(dt('User canonical @oldpath - @newpath', array(
            '@oldpath' => $canonical->field_meta_canonical_url_value,
            '@mail' => $mail
              )
          ) . "\n");
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 *
 */
function yt_update_users_roles() {
  db_truncate('users_roles')->execute();
  $connection = yt_migration_db_connection();
  $query = $connection->select('users_roles', 'ur')->fields('ur', array('rid', 'uid'));
  $query->orderBy('ur.uid', 'ASC');
  $result = $query->execute()->fetchAll();
  foreach ($result as $id => $value) {
    switch ($value->rid) {
      case '3':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 3))->execute();
        $roles[DESTINATION_ROLE_ADMIN] = DESTINATION_ROLE_ADMIN; // 3 - admin , 3 - DESTINATION_ROLE_ADMIN
        break;
      case '4':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 5))->execute();
        $roles[DESTINATION_ROLE_AUTHOR] = DESTINATION_ROLE_AUTHOR; //4 - author , 5 - DESTINATION_ROLE_AUTHOR
        break;
      case '5':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 7))->execute();
        $roles[DESTINATION_ROLE_EDITOR] = DESTINATION_ROLE_EDITOR; //5 - editor , 7 - DESTINATION_ROLE_EDITOR
        break;
      case '7':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 12))->execute();
        $roles[DESTINATION_ROLE_YTEXPERT_DIRECTORY] = DESTINATION_ROLE_YTEXPERT_DIRECTORY; //7 - proconnect-basic, 12 - DESTINATION_ROLE_YTEXPERT_DIRECTORY
        break;
      case '8':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 11))->execute();
        $roles[DESTINATION_ROLE_YTEXPERT_AUTHOR] = DESTINATION_ROLE_YTEXPERT_AUTHOR; //8 - proconnect-advanced, 11 - DESTINATION_ROLE_YTEXPERT_AUTHOR
        break;
      case '9':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 13))->execute();
        $roles[DESTINATION_ROLE_YTEXPERT_PRO] = DESTINATION_ROLE_YTEXPERT_PRO; //9 - proconnect-organization, 13 - DESTINATION_ROLE_YTEXPERT_PRO
        break;
      case '10':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 10))->execute();
        $roles[DESTINATION_ROLE_YTEXPERT_ADMIN] = DESTINATION_ROLE_YTEXPERT_ADMIN; //10 - proconnect-admin, 10 - DESTINATION_ROLE_YTEXPERT_ADMIN
        break;
      case '11':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 6))->execute();
        $roles[DESTINATION_ROLE_BLOGGER] = DESTINATION_ROLE_BLOGGER; //11 - blogger, 6 - DESTINATION_ROLE_BLOGGER
        break;
      case '13':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 4))->execute();
        $roles[DESTINATION_ROLE_AD_ADMIN] = DESTINATION_ROLE_AD_ADMIN; //13 - ads admin, 4 - DESTINATION_ROLE_AD_ADMIN
        break;
      case '14':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 9))->execute();
        $roles[DESTINATION_ROLE_PARTNER] = DESTINATION_ROLE_PARTNER; //14 - partner, 9 - DESTINATION_ROLE_PARTNER
        break;
      case '16':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 8))->execute();
        $roles[DESTINATION_ROLE_G_AUTHOR] = DESTINATION_ROLE_G_AUTHOR; //16 - google author, 8 - DESTINATION_ROLE_G_AUTHOR
        break;
      case '17':
        db_insert('users_roles')->fields(array('uid', 'rid'))->values(array('uid' => $value->uid, 'rid' => 15))->execute();
        $roles[DESTINATION_ROLE_EDITOR_ADMIN] = DESTINATION_ROLE_EDITOR_ADMIN; //17 - producer, 15 - DESTINATION_ROLE_EDITOR_ADMIN
        break;
    }
  }
}

/**
 * Update user information
 */
function yt_migration_revert_userinfo() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $query = db_select('users', 'u')->fields('u');
    $query->range($start, $limit);
    $query->orderBy('u.uid', 'ASC');
    $query->condition('u.uid', '1', '>');
    $users = $query->execute();
    foreach ($users as $id => $user) {
      $name = str_replace('.ebizontek', '', $user->name);
      $mail = str_replace('.ebizon', '', $user->mail);
      $init = str_replace('.ebizon', '', $user->init);
      db_update('users')
          ->fields(array(
            'name' => $name,
            'mail' => $mail,
            'init' => $init,
          ))
          ->condition('uid', $user->uid, '=')
          ->execute();
      drush_print(dt('User updated @oldname - @name - @oldmail - @mail - @oldinit - @init', array('@name' => $name, '@mail' => $mail, '@init' => $init, '@oldname' => $user->name, '@oldmail' => $user->mail, '@oldinit' => $user->init)) . "\n");
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Update user join date
 */
function yt_update_users_join_date() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $query = db_select('profile', 'p')->fields('p');
    $query->range($start, $limit);
    $query->orderBy('p.created', 'ASC');
    $profiles = $query->execute();
    foreach ($profiles as $id => $profile) {
      db_delete('field_data_field_join_date')
          ->condition('entity_id', $profile->pid, '=')
          ->condition('entity_type', 'profile2', '=')
          ->condition('bundle', 'author', '=')
          ->execute();
      db_delete('field_revision_field_join_date')
          ->condition('entity_id', $profile->pid, '=')
          ->condition('entity_type', 'profile2', '=')
          ->condition('bundle', 'author', '=')
          ->execute();

      $created_date = date('Y-m-d H:i:s', $profile->created);
      $field_table_query = db_insert('field_data_field_join_date')
          ->fields(
          array(
            'entity_type',
            'bundle',
            'deleted',
            'entity_id',
            'revision_id',
            'language',
            'delta',
            'field_join_date_value',
          )
      );
      $field_table_query->values(array(
        'entity_type' => 'profile2',
        'bundle' => 'author',
        'deleted' => '0',
        'entity_id' => $profile->pid,
        'revision_id' => $profile->pid,
        'language' => 'und',
        'delta' => '0',
        'field_join_date_value' => $created_date,
      ));
      $field_table_query->execute();

      $field_revision_table_query = db_insert('field_revision_field_join_date')
          ->fields(
          array(
            'entity_type',
            'bundle',
            'deleted',
            'entity_id',
            'revision_id',
            'language',
            'delta',
            'field_join_date_value',
          )
      );
      $field_revision_table_query->values(array(
        'entity_type' => 'profile2',
        'bundle' => 'author',
        'deleted' => '0',
        'entity_id' => $profile->pid,
        'revision_id' => $profile->pid,
        'language' => 'und',
        'delta' => '0',
        'field_join_date_value' => $created_date,
      ));
      $field_revision_table_query->execute();
      drush_log(dt('@title reprisal date get updated.', array('@title' => $node->title)));
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Change the migration status update
 * Experts role users
 */
function yt_migration_update_experts() {
  if (db_table_exists('migrate_map_yourtangoauthor')) {
    db_update('migrate_map_yourtangoauthor')
        ->fields(array(
          'needs_update' => 0,
        ))
        ->execute();
  }
  $query = db_select('users_roles', 'ur')->fields('ur');
  $query->orderBy('ur.uid', 'ASC');
  $experts = $query->execute();
  foreach ($experts as $id => $expert) {
    if (db_table_exists('migrate_map_yourtangoauthor')) {
      db_update('migrate_map_yourtangoauthor')
          ->fields(array(
            'needs_update' => 1,
          ))
          ->condition('sourceid1', $expert->uid, '=')
          ->execute();
    }
  }
}

/**
 * Sanitized the d6 db
 */
function yt_migration_sanitize_d6_db() {
  $connection = yt_migration_db_connection();
}

/**
 * Sanitized the d6 db
 * 'field_coaching_orientation'
 * 'field_primary_type_of_pro', 
 * 'field_type_of_pro'
 * 'field_type_pro'
 * 'field_credentials'
 * 'field_specialties'
 */
function yt_node_fields_updates() {
  $connection = yt_migration_db_connection();
  $fields = array('field_coaching_orientation', 'field_type_pro', 'field_credentials', 'field_specialties');
  $query = $connection->select('content_node_field', 'cn')->fields('cn', array('global_settings', 'field_name'));
  $query->condition('cn.field_name', $fields, 'IN');
  $result = $query->execute()->fetchAll();
  foreach ($result as $field_key => $field) {
    $allow_values = array();
    $allow_values = unserialize($field->global_settings);
    $field_values = $allow_values['allowed_values'];
    $list = explode("\n", $field_values);
    $list = array_map('trim', $list);
    $list = array_filter($list, 'strlen');
    foreach ($list as $position => $text) {
      $values = array();
      $value = $key = FALSE;
      // Check for an explicit key.
      $matches = array();
      if (preg_match('/(.*)\|(.*)/', $text, $matches)) {
        $key = $matches[1];
        $value = $matches[2];
        $explicit_keys = TRUE;
      }
      $values[$key] = $value;
    }

    if (count($values) > 0) {
      if ($field->field_name == 'field_coaching_orientation') {
        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_helping_orientation', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }
      }
      else if ($field->field_name == 'field_type_pro') {

        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_primary_type_of_pro', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }

        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_type_of_pro', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }
      }
      else if ($field->field_name == 'field_specialties') {

        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_my_specialties', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }

        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_main_specialties', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }
      }
      else if ($field->field_name == 'field_credentials') {
        $field_config = db_select('field_config', 'fc')
            ->fields('fc', array('data', 'id'))
            ->condition('fc.field_name', 'field_credentials', '=');
        $field_config_result = $field_config->execute()->fetch();
        if (is_object($field_config_result)) {
          $new_settings = unserialize($field_config_result->data);
          $new_settings['settings']['allowed_values'] = $values;
          db_update('field_config')
              ->fields(array(
                'data' => serialize($new_settings),
              ))
              ->condition('id', $field_config_result->id, '=')
              ->execute();
        }
      }
    }
  }
}

/**
 * Delete blank created success stories
 */
function yt_migration_drush_delete_ss() {
  $query = db_select('field_data_field_success_stories', 'fd')->fields('fd');
  $query->condition('fd.bundle', 'author', '=');
  $query->orderBy('fd.entity_id', 'ASC');
  $collections = $query->execute();
  foreach ($collections as $id => $values) {
    $collection = db_select('field_collection_item', 'fc')
        ->fields('fc', array('item_id', 'revision_id'))
        ->condition('fc.item_id', $values->field_success_stories_value, '=')
        ->execute()
        ->fetchObject();
    if (!is_object($collection) && empty($collection->revision_id)) {
      db_update('field_data_field_success_stories')
          ->fields(array(
            'deleted' => 1,
          ))
          ->condition('entity_id', $values->entity_id, '=')
          ->condition('revision_id', $values->revision_id, '=')
          ->condition('delta', $values->delta, '=')
          ->condition('field_success_stories_value', $values->field_success_stories_value, '=')
          ->execute();
      db_update('field_revision_field_success_stories')
          ->fields(array(
            'deleted' => 1,
          ))
          ->condition('entity_id', $values->entity_id, '=')
          ->condition('revision_id', $values->revision_id, '=')
          ->condition('delta', $values->delta, '=')
          ->condition('field_success_stories_value', $values->field_success_stories_value, '=')
          ->execute();
    }
  }
  drush_log(dt('Success stories updated sucessfully.'));
  return;
}

/**
 * Delete newly created content from d7
 * @param type $type
 */
function yt_migration_drush_delete_content($type = NULL) {
  try {
    $args = drush_get_arguments();
    $timestamp = drush_get_option('timestamp');
    $id = drush_get_option('id');
    if (!empty($timestamp)) {
      $time = time() - $timestamp;
    }
    switch ($type) {
      case 'node':
        $query = db_select('node', 'n')->fields('n');
        $or = db_or()->condition('nid', $id, '>')->condition('changed', $time, ">=");
        $query->condition($or);
        $query->condition('type', 'product_display', '!=');
        $nodes = $query->execute();
        foreach ($nodes as $key => $node) {
          if ($node->nid > $id && $node->nid != '245095') {
            node_delete($node->nid);
            drush_print(dt('Content delete @type node @nid - @title', array('@type' => $node->type, '@nid' => $node->nid, '@title' => $node->title)) . "\n");
          }
          else {
            drush_print(dt('Update @type node @nid - @title', array('@type' => $node->type, '@nid' => $node->nid, '@title' => $node->title)) . "\n");
            switch ($node->type) {
              case 'blog':
                if (db_table_exists('migrate_map_yourtangoblog')) {
                  db_update('migrate_map_yourtangoblog')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangoblog not found'));
                  return;
                }
                break;
              case 'embedded_video':
                if (db_table_exists('migrate_map_yourtangoembbedvideo')) {
                  db_update('migrate_map_yourtangoembbedvideo')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangoembbedvideo not found'));
                  return;
                }
                break;
              case 'external_article':
                if (db_table_exists('migrate_map_yourtangoexternalarticle')) {
                  db_update('migrate_map_yourtangoexternalarticle')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangoexternalarticle not found'));
                  return;
                }
                break;
              case 'feature':
                if (db_table_exists('migrate_map_yourtangofeature')) {
                  db_update('migrate_map_yourtangofeature')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangofeature not found'));
                  return;
                }
                break;
              case 'list':
                if (db_table_exists('migrate_map_yourtangolist')) {
                  db_update('migrate_map_yourtangolist')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangolist not found'));
                  return;
                }
                break;
              case 'lovefact':
                if (db_table_exists('migrate_map_yourtangofactoflove')) {
                  db_update('migrate_map_yourtangofactoflove')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangofactoflove not found'));
                  return;
                }
                break;
              case 'node_gallery_gallery':
                if (db_table_exists('migrate_map_yourtangogallery')) {
                  db_update('migrate_map_yourtangogallery')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangogallery not found'));
                  return;
                }
                break;
              case 'node_gallery_image':
                if (db_table_exists('migrate_map_yourtangogalleryimage')) {
                  db_update('migrate_map_yourtangogalleryimage')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangogalleryimage not found'));
                  return;
                }
                break;
              case 'page':
                if (db_table_exists('migrate_map_yourtangopage')) {
                  db_update('migrate_map_yourtangopage')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangopage not found'));
                  return;
                }
                break;
              case 'partner_link':
                if (db_table_exists('migrate_map_yourtangopartnerlink')) {
                  db_update('migrate_map_yourtangopartnerlink')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangopartnerlink not found'));
                  return;
                }
                break;
              case 'resource':
                if (db_table_exists('migrate_map_yourtangoresources')) {
                  db_update('migrate_map_yourtangoresources')
                      ->fields(array(
                        'needs_update' => 1,
                      ))
                      ->condition('sourceid1', $node->nid, '=')
                      ->condition('destid1', $node->nid, '=')
                      ->execute();
                }
                else {
                  drush_log(dt('Table migrate_map_yourtangoresources not found'));
                  return;
                }
                break;
            }
          }
        }
        break;
      case 'user':
        $query = db_select('users', 'u')->fields('u');
        $query->condition('uid', $id, '>');
        $users = $query->execute();
        foreach ($users as $key => $account) {
          user_delete($account->uid);
          drush_print(dt('User delete @uid - @name', array('@uid' => $account->uid, '@title' => $account->name)) . "\n");
        }
        break;
      case 'taxonomy':
        $query = db_select('taxonomy_term_data', 'td')->fields('td', array('name', 'tid'));
        $query->fields('tv', array('name'));
        $query->join('taxonomy_vocabulary', 'tv', 'tv.vid = td.vid');
        $query->condition('td.tid', $id, '>');
        $query->condition('tv.vid', array('9'), 'IN');
        $taxonomies = $query->execute();
        foreach ($taxonomies as $key => $taxonomy) {
          taxonomy_term_delete($taxonomy->tid);
          drush_print(dt('Term delete @vocabulary, @tid - @term_name', array('@vocabulary' => $taxonomy->tv_name, '@tid' => $taxonomy->tid, '@term_name' => $taxonomy->name)) . "\n");
        }
        break;
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Update Changes Script
 */
function yt_migration_drush_update_changes($type = NULL) {
  try {
    $args = drush_get_arguments();
    $timestamp = drush_get_option('timestamp');
    $id = drush_get_option('id');
    if (!empty($timestamp)) {
      $time = time() - $timestamp;
    }
    switch ($type) {
      case 'node':
        if (isset($timestamp) && !is_numeric($timestamp)) {
          return drush_set_error(dt('"Timestamp" is not a valid statement type. Please run "drush help ytuc" to see the valid parameters.'));
        }
        if (isset($id) && !is_numeric($id)) {
          return drush_set_error(dt('"ID" is not a valid statement type. Please run "drush help ytuc" to see the valid parameters.'));
        }
        $connection = yt_migration_db_connection();
        $content_types = array('author', 'blog', 'embedded_video', 'external_article', 'feature', 'list', 'lovefact', 'node_gallery_gallery', 'node_gallery_image', 'page', 'partner_link', 'resource');
        $query = $connection->select('node', 'n')->fields('n', array('nid', 'type', 'uid', 'title'));
        if (!empty($time)) {
          $query->condition('n.changed', $time, '>=');
        }
        if (!empty($id)) {
          $query->condition('n.nid', $id, '<=');
        }
        $query->condition('n.type', $content_types, 'IN');
        $query->orderBy('n.nid', 'ASC');
        $result = $query->execute()->fetchAll();
        foreach ($result as $key => $node) {
          $node_type = $node->type;
          drush_print(dt('Update @type node @nid - @title', array('@type' => $node_type, '@nid' => $node->nid, '@title' => $node->title)) . "\n");
          switch ($node_type) {
            case 'author':
              if (db_table_exists('migrate_map_yourtangoauthor')) {
                db_update('migrate_map_yourtangoauthor')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->uid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangoauthor not found'));
                return;
              }
              break;
            case 'blog':
              if (db_table_exists('migrate_map_yourtangoblog')) {
                db_update('migrate_map_yourtangoblog')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangoblog not found'));
                return;
              }
              break;
            case 'embedded_video':
              if (db_table_exists('migrate_map_yourtangoembbedvideo')) {
                db_update('migrate_map_yourtangoembbedvideo')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangoembbedvideo not found'));
                return;
              }
              break;
            case 'external_article':
              if (db_table_exists('migrate_map_yourtangoexternalarticle')) {
                db_update('migrate_map_yourtangoexternalarticle')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangoexternalarticle not found'));
                return;
              }
              break;
            case 'feature':
              if (db_table_exists('migrate_map_yourtangofeature')) {
                db_update('migrate_map_yourtangofeature')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangofeature not found'));
                return;
              }
              break;
            case 'list':
              if (db_table_exists('migrate_map_yourtangolist')) {
                db_update('migrate_map_yourtangolist')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangolist not found'));
                return;
              }
              break;
            case 'lovefact':
              if (db_table_exists('migrate_map_yourtangofactoflove')) {
                db_update('migrate_map_yourtangofactoflove')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangofactoflove not found'));
                return;
              }
              break;
            case 'node_gallery_gallery':
              if (db_table_exists('migrate_map_yourtangogallery')) {
                db_update('migrate_map_yourtangogallery')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangogallery not found'));
                return;
              }
              break;
            case 'node_gallery_image':
              if (db_table_exists('migrate_map_yourtangogalleryimage')) {
                db_update('migrate_map_yourtangogalleryimage')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangogalleryimage not found'));
                return;
              }
              break;
            case 'page':
              if (db_table_exists('migrate_map_yourtangopage')) {
                db_update('migrate_map_yourtangopage')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangopage not found'));
                return;
              }
              break;
            case 'partner_link':
              if (db_table_exists('migrate_map_yourtangopartnerlink')) {
                db_update('migrate_map_yourtangopartnerlink')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangopartnerlink not found'));
                return;
              }
              break;
            case 'resource':
              if (db_table_exists('migrate_map_yourtangoresources')) {
                db_update('migrate_map_yourtangoresources')
                    ->fields(array(
                      'needs_update' => 1,
                    ))
                    ->condition('sourceid1', $node->nid, '=')
                    ->condition('destid1', $node->nid, '=')
                    ->execute();
              }
              else {
                drush_log(dt('Table migrate_map_yourtangoresources not found'));
                return;
              }
              break;
          }
        }
        break;
      case 'user':
        $connection = yt_migration_db_connection();
        $query = $connection->select('users', 'u')->fields('u', array('uid', 'name'));
        if (!empty($time)) {
          $query->condition('u.access', $time, '>=');
        }
        if (!empty($id)) {
          $query->condition('u.uid', $id, '<=');
        }
        $query->orderBy('u.uid', 'ASC');
        $result = $query->execute()->fetchAll();
        foreach ($result as $key => $account) {
          drush_print(dt('Update user @uid - @name', array('@uid' => $account->uid, '@name' => $account->name)) . "\n");
          if (db_table_exists('migrate_map_yourtangousers')) {
            db_update('migrate_map_yourtangousers')
                ->fields(array(
                  'needs_update' => 1,
                ))
                ->condition('sourceid1', $account->uid, '=')
                ->condition('destid1', $account->uid, '=')
                ->execute();
          }
          else {
            drush_set_error(dt('Table migrate_map_yourtangousers not found'));
            return;
          }
        }
        break;
      case 'taxonomy':
        $term_tables = array(
          'migrate_map_yourtangotaxonomyblock',
          'migrate_map_yourtangotaxonomyblog',
          'migrate_map_yourtangotaxonomycategory',
          'migrate_map_yourtangotaxonomylovestage',
          'migrate_map_yourtangotaxonomyseries',
          'migrate_map_yourtangotaxonomysuperdupertags',
          'migrate_map_yourtangotaxonomysupertags',
          'migrate_map_yourtangotaxonomyusertags'
        );
        foreach ($term_tables as $table) {
          if (db_table_exists($table)) {
            db_update($table)
                ->fields(array(
                  'needs_update' => 1,
                ))
                ->execute();
          }
          else {
            drush_log(dt('Table @table not found', array('@table' => $table)));
          }
        }
        break;
        case 'updatednodes':
          
          db_update('migrate_map_yourtangoblog')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangoembbedvideo')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangoexternalarticle')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangofeature')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangogallery')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangogalleryimage')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
          
          db_update('migrate_map_yourtangolist')->fields(array('needs_update' => 0))->condition('needs_update', 1, '=')->execute();
		      
		      if (isset($timestamp) && !is_numeric($timestamp)) {
		        return drush_set_error(dt('"Timestamp" is not a valid statement type. Please run "drush help ytuc" to see the valid parameters.'));
		      }
		      if (isset($id) && !is_numeric($id)) {
		        return drush_set_error(dt('"ID" is not a valid statement type. Please run "drush help ytuc" to see the valid parameters.'));
		      }
		      $connection = yt_migration_db_connection();
		      $content_types = array('blog', 'embedded_video', 'external_article', 'feature', 'list', 'node_gallery_gallery', 'node_gallery_image');
		      $query = $connection->select('node', 'n')->fields('n', array('nid', 'type', 'uid', 'title'));
		      if (!empty($time)) {
		        $query->condition('n.changed', $time, '>=');
		      }
		      if (!empty($id)) {
		        $query->condition('n.nid', $id, '<');
		      }
		      $query->condition('n.type', $content_types, 'IN');
		      $query->orderBy('n.nid', 'ASC');
		      $result = $query->execute()->fetchAll();
					foreach ($result as $key => $node) {
		        $node_type = $node->type;
		        drush_print(dt('Update @type node @nid - @title', array('@type' => $node_type, '@nid' => $node->nid, '@title' => $node->title)) . "\n");
		        switch ($node_type) {
		          case 'blog':
		            if (db_table_exists('migrate_map_yourtangoblog')) {
		              db_update('migrate_map_yourtangoblog')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangoblog not found'));
		              return;
		            }
		            break;
		          case 'embedded_video':
		            if (db_table_exists('migrate_map_yourtangoembbedvideo')) {
		              db_update('migrate_map_yourtangoembbedvideo')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangoembbedvideo not found'));
		              return;
		            }
		            break;
		          case 'external_article':
		            if (db_table_exists('migrate_map_yourtangoexternalarticle')) {
		              db_update('migrate_map_yourtangoexternalarticle')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangoexternalarticle not found'));
		              return;
		            }
		            break;
		          case 'feature':
		            if (db_table_exists('migrate_map_yourtangofeature')) {
		              db_update('migrate_map_yourtangofeature')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangofeature not found'));
		              return;
		            }
		            break;
		          case 'list':
		            if (db_table_exists('migrate_map_yourtangolist')) {
		              db_update('migrate_map_yourtangolist')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangolist not found'));
		              return;
		            }
		            break;
		          case 'node_gallery_gallery':
		            if (db_table_exists('migrate_map_yourtangogallery')) {
		              db_update('migrate_map_yourtangogallery')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangogallery not found'));
		              return;
		            }
		            break;
		          case 'node_gallery_image':
		            if (db_table_exists('migrate_map_yourtangogalleryimage')) {
		              db_update('migrate_map_yourtangogalleryimage')
		                  ->fields(array(
		                    'needs_update' => 1,
		                  ))
		                  ->condition('sourceid1', $node->nid, '=')
		                  ->condition('destid1', $node->nid, '=')
		                  ->execute();
		            }
		            else {
		              drush_log(dt('Table migrate_map_yourtangogalleryimage not found'));
		              return;
		            }
		            break;
		        }
		      }     
        break;
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Migration script for proximity search
 */
function yt_migration_drush_latlon() {
  module_load_include('inc', 'geocoder', 'plugins/geocoder_handler/google');
  include_once(drupal_get_path('module', 'geofield') . '/includes/GeoGenerator.php');
  $all_entity_info = entity_get_info();
  foreach ($all_entity_info as $entity_type => $entity_info) {
    if ($entity_type == 'profile2') {
      if ($entity_info['fieldable']) {
        foreach ($entity_info['bundles'] as $bundle_name => $bundle_info) {
          foreach (field_info_instances($entity_type, $bundle_name) as $field_name => $field_instance) {
            $field_info = field_info_field($field_name);
            if ($field_instance['widget']['type'] === 'geocoder') {
              $entity_load = $entity_info['load hook'];
              $query = db_select($entity_info['base table'])
                  ->fields($entity_info['base table'], array($entity_info['entity keys']['id']))
                  ->condition($entity_info['entity keys']['bundle'], $bundle_name);
              $results = $query->execute();
              while ($id = $results->fetchField()) {
                $profile_array = array();
                if (in_array($id, $profile_array)) {
                  $entity = $entity_load($id);
                  $langcode = field_language($entity_type, $entity, $field_name);
                  $items = field_get_items($entity_type, $entity, $field_name, $langcode);
                  if (count($entity->field_office_address)) {
                    $field_item = array();
                    $options = array();
                    $geometries = array();
                    if ($field_info['type'] == 'geofield') {
                      geophp_load();
                      $field_item['name'] = $entity->field_office_address[LANGUAGE_NONE][0]['name'];
                      $field_item['street'] = $entity->field_office_address[LANGUAGE_NONE][0]['street'];
                      $field_item['additional'] = $entity->field_office_address[LANGUAGE_NONE][0]['additional'];
                      $field_item['city'] = $entity->field_office_address[LANGUAGE_NONE][0]['city'];
                      $field_item['province'] = $entity->field_office_address[LANGUAGE_NONE][0]['province'];
                      $field_item['country'] = $entity->field_office_address[LANGUAGE_NONE][0]['country'];
                      $field_item['postal_code'] = $entity->field_office_address[LANGUAGE_NONE][0]['postal_code'];
                      $options['geometry_type'] = 'viewport';
                      $options['all_results'] = '1';
                      $options['reject_results'] = array('APPROXIMATE' => 0, 'GEOMETRIC_CENTER' => 0, 'RANGE_INTERPOLATED' => 0, 'ROOFTOP' => 0);
                      $address = geocoder_widget_parse_locationfield($field_item);
                      $geometry = geocoder_google($address, $options);
                      if ($geometry instanceof Geometry) {
                        $geometries[] = $geometry;
                      }
                      $target_info = field_info_field($field_instance['field_name']);
                      $target_type = $target_info['type'];
                      $values = geocoder_widget_resolve_deltas($geometries, 'default', $target_type);
                      //$field_value['geohash'] = $point['geohash'];
                      $entity->field_latlong = array(LANGUAGE_NONE => $values);
                      //$entity->original = array();
                      entity_save($entity_type, $entity);
                      /* db_merge('field_data_field_latlong')
                        ->fields(array(
                        'geom' => $values['0']['geom'],
                        'geo_type' => $values['0']['geo_type'],
                        'lat' => $values['0']['lat'],
                        'lon' => $values['0']['lon'],
                        'top' => $values['0']['top'],
                        'bottom' => $values['0']['bottom'],
                        'right' => $values['0']['right'],
                        'left' => $values['0']['left'],
                        'geohash' => $values['0']['geohash']
                        ))
                        ->key(array(
                        'entity_type' => 'profile2',
                        'bundle' => 'author',
                        'deleted' => '0',
                        'revision_id' => $entity->pid,
                        'entity_id' => $entity->pid,
                        'delta' => '0',
                        'language' => LANGUAGE_NONE,
                        ))->execute();

                        db_merge('field_revision_field_latlong')
                        ->key(array(
                        'entity_type' => 'profile2',
                        'bundle' => 'author',
                        'deleted' => '0',
                        'revision_id' => $entity->pid,
                        'entity_id' => $entity->pid,
                        'delta' => '0',
                        'language' => LANGUAGE_NONE,
                        ))
                        ->fields(array(
                        'geom' => $values['0']['geom'],
                        'geo_type' => $values['0']['geo_type'],
                        'lat' => $values['0']['lat'],
                        'lon' => $values['0']['lon'],
                        'top' => $values['0']['top'],
                        'bottom' => $values['0']['bottom'],
                        'right' => $values['0']['right'],
                        'left' => $values['0']['left'],
                        'geohash' => $values['0']['geohash'],
                        ))
                        ->execute(); */
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  cache_clear_all('*', 'cache_entity_profile2', TRUE);
}

/**
 * Migration script for Reference Images
 */
function yt_migration_drush_ref_image() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('title', 'uid', 'type'))
        ->fields('f', array('filename', 'fid', 'filepath'))
        ->fields('ctb', array('nid', 'field_referenced_image_nid'))
        ->condition('ctb.nid', '40114', '>=')
        ->condition('im.image_size', '_original', '=');
    $query->join('content_type_blog', 'ctb', 'n.nid = ctb.field_referenced_image_nid');
    $query->join('image', 'im', 'im.nid = ctb.field_referenced_image_nid');
    $query->join('files', 'f', 'f.fid = im.fid');
    $query->isNull('ctb.field_image_blog_fid');
    $query->orderBy('ctb.nid', 'DESC');
    $query->range($start, $limit);
    $result = $query->execute()->fetchAll();
    foreach ($result as $id => $value) {
      $entity = node_load($value->nid);
      if (is_object($entity)) {
        $reference = db_select('migrate_map_yourtangofile', 'mf')
            ->fields('mf', array('destid1'))
            ->condition('mf.sourceid1', $value->fid, '=');
        $ref_image = $reference->execute()->fetch();
        if ($ref_image->destid1 > 0) {
          $file = file_load($ref_image->destid1);
          $entity->field_image_blog[LANGUAGE_NONE][0] = (array) $file;
          node_save($entity);
        }
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Migrating user alias
 */
function yt_migration_drush_user_alias() {
  $connection = yt_migration_db_connection();
  $query = $connection->select('url_alias', 'ua')
      ->fields('ua', array('src', 'dst'))
      ->condition('ua.src', 'user/%', 'LIKE')
      ->condition('ua.src', '%/contact', 'NOT LIKE')
      ->condition('ua.src', '%/track', 'NOT LIKE')
      ->condition('ua.src', '%/track%', 'NOT LIKE')
      ->condition('ua.src', '%/feed', 'NOT LIKE');
  $query->orderBy('ua.pid', 'ASC');
  $result = $query->execute()->fetchAll();
  foreach ($result as $id => $value) {
    $path = array();
    $source = $value->src;
    db_delete('url_alias')->condition('source', $source, '=')->execute();
    $path['source'] = $value->src;
    $path['alias'] = $value->dst;
    $path['language'] = LANGUAGE_NONE;
    drupal_write_record('url_alias', $path);
  }
}

/**
 * Reprisal date update old content
 */
function yt_migration_reprisal_date_update() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $query = db_select('node', 'n')->fields('n');
    $query->range($start, $limit);
    $query->orderBy('n.created', 'ASC');
    $nodes = $query->execute();
    foreach ($nodes as $id => $node) {
      /* $check_reprisal_date = db_select('field_data_field_reprisal_date', 'rd')
        ->fields('rd', array('field_reprisal_date_value'))
        ->condition('rd.entity_id', $node->nid, '=')
        ->condition('rd.entity_type', 'node', '=')
        ->condition('rd.bundle', $node->type, '=');
        $check_reprisal_date_result = $check_reprisal_date->execute()->fetch();
        if (!is_object($check_reprisal_date_result)) { */
      db_delete('field_revision_field_reprisal_date')
          ->condition('entity_id', $node->nid, '=')
          ->condition('entity_type', 'node', '=')
          ->condition('bundle', $node->type, '=')
          ->execute();
      db_delete('field_data_field_reprisal_date')
          ->condition('entity_id', $node->nid, '=')
          ->condition('entity_type', 'node', '=')
          ->condition('bundle', $node->type, '=')
          ->execute();

      $created_date = date('Y-m-d H:i:s', $node->created);
      $field_table_query = db_insert('field_data_field_reprisal_date')
          ->fields(
          array(
            'entity_type',
            'bundle',
            'deleted',
            'entity_id',
            'revision_id',
            'language',
            'delta',
            'field_reprisal_date_value',
          )
      );
      $field_table_query->values(array(
        'entity_type' => 'node',
        'bundle' => $node->type,
        'deleted' => '0',
        'entity_id' => $node->nid,
        'revision_id' => $node->vid,
        'language' => 'und',
        'delta' => '0',
        'field_reprisal_date_value' => $created_date,
      ));
      $field_table_query->execute();

      $field_revision_table_query = db_insert('field_revision_field_reprisal_date')
          ->fields(
          array(
            'entity_type',
            'bundle',
            'deleted',
            'entity_id',
            'revision_id',
            'language',
            'delta',
            'field_reprisal_date_value',
          )
      );
      $field_revision_table_query->values(array(
        'entity_type' => 'node',
        'bundle' => $node->type,
        'deleted' => '0',
        'entity_id' => $node->nid,
        'revision_id' => $node->vid,
        'language' => 'und',
        'delta' => '0',
        'field_reprisal_date_value' => $created_date,
      ));
      $field_revision_table_query->execute();
      drush_log(dt('@title reprisal date get updated.', array('@title' => $node->title)));
      //}
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Updating the content aliases 
 * from current d6 databases;
 */
function yt_migration_update_content_alias() {
  $query = db_select('node', 'n')->fields('n');
  $query->orderBy('n.created', 'ASC');
  $nodes = $query->execute();
  foreach ($nodes as $id => $node) {
    $alias = yt_migration_get_url_alias($node->nid);
    if (!empty($alias)) {
      db_delete('url_alias')->condition('source', 'node/' . $node->nid)->execute();
      $main_alias = array('source' => 'node/' . $node->nid, 'alias' => $alias);
      path_save($main_alias);
    }
  }
}

function yt_migration_drush_matatags_node_types() {
  global $base_url;
  /*   * * Node Meta Tags ** */
  try {
    $connection = yt_migration_db_connection();
    $node_tags = $connection->select('tango_meta', 'tm')
        ->fields('tm', array('object_id', 'keywords', 'description', 'title', 'canonical_url', 'website_name'))
        ->condition('tm.object_type', 1, '=')
        ->condition('pt.type', 'node', '=')
        ->condition('n.type', array('embedded_video', 'list', 'node_gallery_gallery'), 'IN');
    $node_tags->join('node', 'n', 'tm.object_id = n.nid');
    $node_tags->leftJoin('page_title', 'pt', 'pt.id = n.nid');
    $node_tags->orderBy('n.created', 'ASC');
    $node_tags_result = $node_tags->execute()->fetchAll();
    foreach ($node_tags_result as $id => $value) {
      $node = db_select('node', 'n')
          ->fields('n', array('nid', 'vid'))
          ->condition('n.nid', $value->object_id, '=');
      $node_result = $node->execute()->fetch();
      if (is_object($node_result)) {
        $allow_index = array(
          'index' => 'index',
          'follow' => 'follow',
          'noindex' => '0',
          'nofollow' => '0',
          'noarchive' => '0',
          'nosnippet' => '0',
          'noodp' => 'noodp',
          'noydir' => 'noydir',
          'noimageindex' => '0',
          'notranslate' => '0',
        );
        db_delete('metatag')
            ->condition('entity_type', 'node')
            ->condition('entity_id', $node_result->nid)
            ->condition('revision_id', $node_result->vid)
            ->execute();

        $new_metatags = array();

        $meta_title = yt_migration_get_metatag_title($node_result->nid);
        $new_metatags['title']['value'] = !empty($meta_title) ? $meta_title : '';
        $new_metatags['description']['value'] = !empty($value->description) ? $value->description : '';
        //$new_metatags['abstract']['value'] = !empty($row->metatag_abstract) ? $row->metatag_abstract : '';
        $new_metatags['canonical']['value'] = $base_url . '/' . drupal_get_path_alias('node/' . $node_result->nid);
        $new_metatags['robots']['value'] = $allow_index;
        $new_metatags['keywords']['value'] = !empty($value->keywords) ? $value->keywords : '';

        db_merge('metatag')
            ->key(array(
              'entity_type' => 'node',
              'entity_id' => $node_result->nid,
              'language' => 'und',
              'revision_id' => $node_result->vid,
            ))
            ->fields(array(
              'data' => serialize($new_metatags),
            ))
            ->execute();
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Migrate meta Tags
 */
function yt_migration_drush_matatags() {
  /*   * * Node Meta Tags ** */
  try {
    global $base_url;
    $connection = yt_migration_db_connection();
    $args = drush_get_arguments();
    $entity = drush_get_option('entity');
    $type = drush_get_option('type');
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');

    //db_delete('metatag')->condition('language', 'und', '!=')->execute();
    if ($entity == 'node') {
      $node_tags = $connection->select('tango_meta', 'tm')
          ->fields('tm', array('object_id', 'keywords', 'description', 'title', 'canonical_url', 'website_name'))
          ->fields('n', array('nid', 'vid'))
          ->condition('tm.object_type', 1, '=');
      if (!empty($type)) {
        $node_tags->condition('n.type', $type, '=');
      }
      $node_tags->join('node', 'n', 'tm.object_id = n.nid');
      $node_tags->range($start, $limit);
      $node_tags->orderBy('n.nid', 'ASC');
      $node_tags_result = $node_tags->execute()->fetchAll();
      foreach ($node_tags_result as $id => $value) {
      	$content_approved = yt_migration_approved_content($value->object_id);
      	$exclusive = yt_migration_tango_exclusive($value->object_id);
        $node = db_select('node', 'n')
            ->fields('n', array('nid', 'vid'))
            ->condition('n.nid', $value->object_id, '=');
        $node_result = $node->execute()->fetch();
    		drush_print(dt('Node @nid - @vid', array('@nid' => $node_result->nid, '@vid' => $node_result->vid)) . "\n");
        if (is_object($node_result)) {
          $exclusive_yourtango = ($exclusive->object_id && !$exclusive->canonical_url) ? 1 : 0;
          $field_approved_value = $content_approved;
          if ($exclusive_yourtango == 1 && $field_approved_value == 1) {
            $allow_index = array(
              'index' => 'index',
              'follow' => 'follow',
              'noindex' => '0',
              'nofollow' => '0',
              'noarchive' => '0',
              'nosnippet' => '0',
              'noodp' => 'noodp',
              'noydir' => 'noydir',
              'noimageindex' => '0',
              'notranslate' => '0',
            );
          }
          else if ($exclusive_yourtango == 1 && $field_approved_value == 0) {
            $allow_index = array(
              'noindex' => 'noindex',
              'nofollow' => 'nofollow',
              'noarchive' => '0',
              'nosnippet' => '0',
              'noodp' => 'noodp',
              'noydir' => 'noydir',
              'noimageindex' => '0',
              'notranslate' => '0',
              'index' => '0',
              'follow' => '0',
            );
          }
          else if ($exclusive_yourtango == 0 && $field_approved_value == 1) {
            $allow_index = array(
              'index' => 'index',
              'noindex' => '0',
              'nofollow' => 'nofollow',
              'follow' => '0',
              'noarchive' => '0',
              'nosnippet' => '0',
              'noodp' => 'noodp',
              'noydir' => 'noydir',
              'noimageindex' => '0',
              'notranslate' => '0',
            );
          }
          else if ($exclusive_yourtango == 0 && $field_approved_value == 0) {
            $allow_index = array(
              'noindex' => 'noindex',
              'nofollow' => 'nofollow',
              'noarchive' => '0',
              'nosnippet' => '0',
              'noodp' => 'noodp',
              'noydir' => 'noydir',
              'noimageindex' => '0',
              'notranslate' => '0',
              'index' => '0',
              'follow' => '0',
            );
          }

          db_delete('metatag')
              ->condition('entity_type', 'node')
              ->condition('entity_id', $node_result->nid)
              ->condition('revision_id', $node_result->vid)
              ->execute();

          $new_metatags = array();

          $meta_title = yt_migration_get_metatag_title($node_result->nid);
          $new_metatags['title']['value'] = !empty($meta_title) ? $meta_title : '';
          $new_metatags['description']['value'] = !empty($value->description) ? $value->description : '';
          //$new_metatags['abstract']['value'] = !empty($row->metatag_abstract) ? $row->metatag_abstract : '';
          $new_metatags['canonical']['value'] = '';
          $new_metatags['robots']['value'] = $allow_index;
          $new_metatags['keywords']['value'] = !empty($value->keywords) ? $value->keywords : '';
					db_insert('metatag')->fields(
						array(
						  'entity_type' => 'node',
							'entity_id' => $node_result->nid,
							'revision_id' => $node_result->vid,
							'language' => 'und',
							'data' => serialize($new_metatags),
						))
					->execute();
        }
      }
    }
    else if ($entity == 'taxonomy') {
      /** Terms Meta Tags  ** */
      $taxonomy_term = $connection->select('tango_meta', 'tm')
          ->fields('tm', array('object_id', 'keywords', 'description', 'title', 'canonical_url', 'website_name'))
          ->condition('tm.object_type', 2, '=');
      $taxonomy_term->join('term_data', 'td', 'tm.object_id = td.tid');
      $taxonomy_term->orderBy('td.tid', 'ASC');
      $taxonomy_term_tags_result = $taxonomy_term->execute()->fetchAll();
      foreach ($taxonomy_term_tags_result as $id => $value) {
        $taxonomy = db_select('taxonomy_term_data', 'td')
            ->fields('td', array('tid', 'vid'))
            ->condition('td.tid', $value->object_id, '=');
        $taxonomy_result = $taxonomy->execute()->fetch();
        if (is_object($taxonomy_result)) {
          //Love Stages
          if ($taxonomy_result->vid == '5') {
            $index_follow = $connection->select('url_alias', 'ua')
                ->fields('ua', array('pid'))
                ->condition('ua.src', 'love-stage/' . $taxonomy_result->tid, '=');
            $index_follow_result = $index_follow->execute()->fetch();
            if (is_object($index_follow_result)) {
              $allow_index = array(
                'index' => 'index',
                'follow' => 'follow',
                'noindex' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
            else {
              $allow_index = array(
                'noindex' => 'noindex',
                'follow' => 'follow',
                'index' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
          }
          else if ($taxonomy_result->vid == '3') {
            //Blogs 
            $index_follow = $connection->select('url_alias', 'ua')
                ->fields('ua', array('pid'))
                ->condition('ua.src', 'blogs/' . $taxonomy_result->tid, '=');
            $index_follow_result = $index_follow->execute()->fetch();
            if (is_object($index_follow_result)) {
              $allow_index = array(
                'index' => 'index',
                'follow' => 'follow',
                'noindex' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
            else {
              $allow_index = array(
                'noindex' => 'noindex',
                'follow' => 'follow',
                'index' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
          }
          else {
            $index_follow = $connection->select('url_alias', 'ua')
                ->fields('ua', array('pid'))
                ->condition('ua.src', 'category/' . $taxonomy_result->tid, '=');
            $index_follow_result = $index_follow->execute()->fetch();
            if (is_object($index_follow_result)) {
              $allow_index = array(
                'index' => 'index',
                'follow' => 'follow',
                'noindex' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
            else {
              $allow_index = array(
                'noindex' => 'noindex',
                'follow' => 'follow',
                'index' => '',
                'nofollow' => '',
                'noarchive' => '',
                'nosnippet' => '',
                'noodp' => 'noodp',
                'noydir' => 'noydir',
                'noimageindex' => '',
                'notranslate' => '',
              );
            }
          }

          db_delete('metatag')
              ->condition('entity_type', 'taxonomy_term')
              ->condition('entity_id', $taxonomy_result->tid)
              ->execute();

          $new_metatags = array();

          $new_metatags['title']['value'] = !empty($value->title) ? $value->title : '';
          $new_metatags['description']['value'] = !empty($value->description) ? $value->description : '';
          $new_metatags['canonical']['value'] = $base_url . '/' . drupal_get_path_alias('taxonomy/term/' . $value->tid);
          $new_metatags['robots']['value'] = $allow_index;
          $new_metatags['keywords']['value'] = !empty($value->keywords) ? $value->keywords : '';

          db_merge('metatag')
              ->key(array(
                'entity_type' => 'taxonomy_term',
                'entity_id' => $taxonomy_result->tid,
                'language' => 'und',
                'revision_id' => 0,
              ))
              ->fields(array(
                'data' => serialize($new_metatags),
              ))
              ->execute();
        }
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Delete unused redirect
 */
function yt_redirect_unused_delete() {
  db_delete('redirect')
      ->condition('redirect', 'node/%', 'LIKE')
      ->execute();
  db_delete('redirect')
      ->condition('source', 'video/%', 'LIKE')
      ->execute();
}

/**
 * Files Migration
 */
function yt_migration_files_migration() {
  /*   * * Node Meta Tags ** */
  try {
    $args = drush_get_arguments();
    $fid = drush_get_option('fid');
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $connection = yt_migration_db_connection();
    $files_query = $connection->select('files', 'f')
        ->fields('f', array('fid', 'uid', 'filename', 'filepath', 'filemime', 'filesize', 'status', 'timestamp'));
    if (!empty($fid)) {
      $files_query->condition('f.fid', $fid, '=');
    }
    else {
      $files_query->range($start, $limit);
    }
    $files_query->orderBy('f.fid', 'ASC');
    $files_result = $files_query->execute()->fetchAll();
    foreach ($files_result as $id => $files) {
      $url = 'http://yourtango.com/' . $files->filepath;
      $uri = str_replace('sites/default/files/', 'public://', $files->filepath);
      yt_migration_download_external_file($url, $uri);
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Files Migration
 */
function yt_migration_d7_files_migration() {
  /*   * * Node Meta Tags ** */
  try {
    $args = drush_get_arguments();
    $fid = drush_get_option('fid');
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
  	$query = db_select('file_managed', 'f')->fields('f');
  	if(isset($fid) && $fid > 0) {
    	$query->condition('f.fid', $fid, '>');
    }	else {
      $query->range($start, $limit);
    }
  	$query->orderBy('f.fid', 'DESC');
  	$files_result = $query->execute();
    foreach ($files_result as $id => $files) {
    	if(strpos($files->uri, 'temporary://') === false) {
		    $uri = str_replace('public://', 'sites/default/files/', $files->uri);
				$url = 'http://yourtango.com/' . $uri;      
		    yt_migration_download_external_file($url, $files->uri);
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Files Migration
 */
function yt_migration_picture_migration() {
  $connection = yt_migration_db_connection();
  $picture_query = $connection->select('users', 'u')
      ->fields('u', array('picture', 'uid'))
      ->condition('u.uid', '126930', '>');
  $picture_result = $picture_query->execute()->fetchAll();
  foreach ($picture_result as $id => $picture) {
    if (strpos($picture->picture, 'sites/all/themes/pinkduck/i/avatars') !== FALSE) {
      $picture_path = 'sites/default/files/' . $picture->picture;
    }
    else {
      $picture_path = $picture->picture;
    }
    $url = 'http://yourtango.com/' . $picture->picture;
    $uri = str_replace('sites/default/files/', 'public://', $picture_path);
    yt_migration_download_external_file($url, $uri);
  }
}

/**
 *
 * param string $url
 *    Full url to file to download
 * param string $uri
 *    Drupal uri of where to save file public://archive/test.jpg
 * param int $save_mode
 *    File save mode from drupal core, ex FILE_EXISTS_REPLACE
 */
function yt_migration_download_external_file($url, $uri, $save_mode = FILE_EXISTS_REPLACE, $manage_file = TRUE) {

  $url_info = parse_url($url);
  $url_path_info = pathinfo($url_info['path']);

  //This helps with filenames with spaces
  $url = $url_info['scheme'] . '://' . $url_info['host'] . $url_path_info['dirname'] . '/' . rawurlencode($url_path_info['basename']);

  //Need to remove the filename from the uri
  $uri_target = file_uri_target($uri);
  $uri_scheme = file_uri_scheme($uri);
  $uri_path_info = pathinfo($uri_target);
  $directory = file_stream_wrapper_uri_normalize($uri_scheme . "://" . $uri_path_info['dirname']);

  if (file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
    $drupal_result = drupal_http_request($url);
    if (!empty($drupal_result->data)) {
      $path = file_stream_wrapper_uri_normalize($uri);
      if ($manage_file) {
        $new_file = file_save_data($drupal_result->data, $path, $save_mode);
      }
      else {
        return file_unmanaged_save_data($drupal_result->data, $path, $save_mode);
      }
    }
    else {
      drush_print(dt("Error downloading file, no data recieved for @url", array('@url' => $url)) . "\n");
      return FALSE;
    }
    drush_print(dt('Files @url - @uri', array('@url' => $url, '@uri' => $uri)) . "\n");
    $new_file->display = 1;
    return (array) $new_file;
  }
  else {
    drush_print(dt("Could not create directory") . "\n");
  }
}

/**
 * Delete All old revisions of the node
 */
function yt_migration_delete_all_revisions() {
  try {
    $args = drush_get_arguments();
    $limit = drush_get_option('limit');
    $start = drush_get_option('start');
    $nodes = db_select('node', 'n')->fields('n', array('nid', 'vid'));
    $nodes->range($start, $limit);
    $nodes->orderBy('n.created', 'ASC');
    $node_results = $nodes->execute()->fetchAll();
    foreach ($node_results as $node) {
      $revisions = db_select('node_revision', 'n')
          ->fields('n', array('vid'))
          ->condition('n.nid', $node->nid)
          ->condition('n.vid', $node->vid, '<')
          ->execute();
      foreach ($revisions as $rev) {
        node_revision_delete($rev->vid);
      }
    }
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

function yt_pending_users_list() {
 try {
 	$directory = file_stream_wrapper_get_instance_by_uri('public://')->getDirectoryPath() .'/yt_migration';
 	if (file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
  	$user_list = 'users-list-'.date('d-m-Y').'.csv';
  	$user_list_filepath = $directory .'/'. $user_list;
		$args = drush_get_arguments();
		$uid = drush_get_option('uid');
		$limit = drush_get_option('limit');
		$start = drush_get_option('start');  
		$connection = yt_migration_db_connection();
		$user_query = $connection->select('users', 'u')
		   ->fields('u', array('uid', 'name', 'mail', 'status', 'created', 'access', 'login', 'picture', 'data'))
		   ->fields('r', array('name'))
		   ->condition('u.uid', $uid, '>');
    $user_query->leftJoin('users_roles', 'ur', 'ur.uid = u.uid');
		$user_query->leftJoin('role', 'r', 'r.rid = ur.rid');       
		$user_query->range($start, $limit);    
		$user_query->orderBy('u.created', 'ASC');
		$users_results = $user_query->execute()->fetchAll();
		if ($handle = fopen($user_list_filepath, 'w')) {		
			foreach($users_results as $key => $account) {
				$mail = str_replace(".ebizon", "", $account->mail);
					      $user_check = db_select('users', 'u')
		          ->fields('u', array('uid'))
		          ->condition('u.mail', $mail, '=');
		    $user_check_result = $user_check->execute()->fetch();
		    if(!isset($user_check_result->uid) && empty($user_check_result->uid)) {
				  $user_details = array();
				  $user_details['uid'] = $account->uid;
				  $user_details['name'] = $account->name;
				  $user_details['mail'] = $mail;
				  $user_details['role'] = !empty($account->r_name) ? $account->r_name : 'authenticated user';
				  $user_details['status'] = $account->status;
				  $user_details['created'] = date("Y-m-d H:i:s", $account->created);
				  $user_details['access'] = ($account->access != 0) ? date("Y-m-d H:i:s", $account->access) : '0';
				  $user_details['login'] = ($account->login != 0) ? date("Y-m-d H:i:s", $account->login) : '0';
				  //$data = unserialize($account->data);
					fputcsv($handle, $user_details);
		    }
			}
			fclose($handle);
		}
 	} 
 }
 catch (MigrateException $e) {
   drush_print($e->getMessage());
   exit;
 }
}

/**
 * Delete all duplicate aliases
 * of the gallery item
 */
function yt_delete_duplicate_aliases() {
  try {
  	$directory = file_stream_wrapper_get_instance_by_uri('public://')->getDirectoryPath() .'/yt_migration';
  	if (file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
  		///$redirect_filename = 'redirect-path-rows-'.date('d-m-Y').'.csv';
  		$delete_filename = 'multiple_path_rows_'.date('d-m-Y').'.csv';
  		////$redirect_filepath = $dir .'/'. $redirect_filename;
  		$delete_filepath = $directory .'/'. $delete_filename;
  		$writable = is_writable(drupal_realpath($directory) == FALSE ? $directory : drupal_realpath($directory));
		  $args = drush_get_arguments();
		  $limit = drush_get_option('limit');
		  $start = drush_get_option('start');  
			$connection = yt_migration_db_connection();
			$gitem_query = $connection->select('node', 'n')
		    ->fields('n', array('nid', 'vid'))
		    ->condition('n.type', 'node_gallery_image', '=')
		    ->condition('n.nid', '274153', '<=')
				->range($start, $limit)    
		    ->orderBy('n.created', 'DESC');
			$gitem_results = $gitem_query->execute()->fetchAll();
			if ($handle = fopen($delete_filepath, 'w')) {
				foreach ($gitem_results as $gitem) {
					$path_alias = yt_migration_get_url_alias($gitem->nid);
					$source_path = 'node/'.$gitem->nid;
					$query = db_select('url_alias', 'ua')->fields('ua')->condition('ua.source', $source_path, '=');
					if(isset($path_alias) && !empty($path_alias)) {
						$query->condition('ua.alias', $path_alias, '!=');
					}
					$query->orderBy('ua.pid', 'ASC');
					$old_aliases = $query->execute()->fetchAll();
					foreach ($old_aliases as $old_alias) {
						$row_data = array();
						$row_data = (array) $old_alias;
						$row_data['current'] = $path_alias;
						$row_data['action'] = 'Delete';
						db_delete('url_alias')->condition('pid', $old_alias->pid, '=')->execute();
						fputcsv($handle, $row_data);
						drush_log(dt('@alias get deleted @source - @real.', array(
							'@alias' => $old_alias->alias, 
							'@source' => $old_alias->source, 
							'@real' => $path_alias
							)
						));
					}
					//Delete the duplicate aliases
		    	db_delete('url_alias')->condition('source', $source_path, '=')->execute();
		    	if(isset($path_alias) && !empty($path_alias)) {
		    		//Save one real alias in the system
				 		$main_alias = array('source' => $source_path, 'alias' => $path_alias);
				  	path_save($main_alias);
		    	} 					
				}
  			fclose($handle);	
  		}
  	}
  }
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }
}

/**
 * Redirect old url to new url
 */
function yt_old_url_redirect() {
  try {
  	$directory = file_stream_wrapper_get_instance_by_uri('public://')->getDirectoryPath() .'/yt_migration';
  	if (file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
  		$redirect_filename = 'redirect-path-rows-'.date('d-m-Y').'.csv';
  		$delete_filename = 'multiple-path-rows-'.date('d-m-Y').'.csv';
  		$redirect_filepath = $directory .'/'. $redirect_filename;
  		$delete_filepath = $directory .'/'. $delete_filename;  
		  $args = drush_get_arguments();
		  $limit = drush_get_option('limit');
		  $start = drush_get_option('start');
		  $content_types = array('blog', 'embedded_video', 'external_article', 'feature', 'list', 'node_gallery_gallery');
			$connection = yt_migration_db_connection();
			$gitem_query = $connection->select('node', 'n')
		    ->fields('n', array('nid', 'vid'))
		    ->condition('n.type', $content_types, 'IN')
		    ->condition('n.nid', '274153', '<=')
				->range($start, $limit)    
		    ->orderBy('n.created', 'DESC');
			$gitem_results = $gitem_query->execute()->fetchAll();
			foreach ($gitem_results as $gitem) {
				$path_alias = yt_migration_get_url_alias($gitem->nid);
				$source_path = 'node/'.$gitem->nid;
				if(isset($path_alias) && !empty($path_alias)) {
					$query = db_select('url_alias', 'ua')->fields('ua')->condition('ua.source', $source_path, '=')->condition('ua.alias', $path_alias, '!=');
					$query->orderBy('ua.pid', 'ASC');
					$old_aliases = $query->execute()->fetchAll();
				  foreach ($old_aliases as $old_alias) {
				  	redirect_delete_by_path($old_alias->alias);
						$redirect = new stdClass();
		 			  redirect_object_prepare($redirect,
				  		array(
				  			'source' => $old_alias->alias,
				  			'source_options' => array(),
				  			'redirect' => $old_alias->source,
				  			'redirect_options' => array(),
				  			'language' => LANGUAGE_NONE,
				  			'uid' => 1,
				 			)
			 			);
			 	 		redirect_save($redirect);
			 	 		$row_data = array();
						$deleta_data = array();
		 	 			$row_data['rid'] = $redirect->rid;
		 	 			$row_data['source'] = $redirect->source;
		 	 			$row_data['redirect'] = $redirect->redirect;
		 	 			$row_data['current'] = $path_alias;		 	
		 	 			$row_data['action'] = 'Redirect';
		 	 			if ($handle_redirect = fopen($redirect_filepath, 'a')) {		 	 						 	 				
		 	 				fputcsv($handle_redirect, $row_data);
		 	 				fclose($handle_redirect);
		 	 			}
		 	 			if ($handle_delete = fopen($delete_filepath, 'a')) {
			 	 			db_delete('url_alias')->condition('pid', $old_alias->pid, '=')->execute();
			 	 			$deleta_data = (array) $old_alias;
			 	 			$row_data['current'] = $path_alias;		 	
			 	 			$row_data['action'] = 'Delete';			 	 				
			 	 			fputcsv($handle_delete, $deleta_data);
			 	 			fclose($handle_delete);
		 	 			}
			 }
			 if(isset($path_alias) && !empty($path_alias)) {
			 	//Delete the duplicate aliases
			 	db_delete('url_alias')->condition('source', $source_path, '=')->execute();
			 	//Save the one real alias in the system
			 	$main_alias = array('source' => $source_path, 'alias' => $path_alias);
			 	path_save($main_alias);
			}
		}
	}
}
	}
  catch (MigrateException $e) {
    drush_print($e->getMessage());
    exit;
  }  	
}

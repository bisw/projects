<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing users from d6 to d7.
 */
class YourTangoPendingUsersMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate Pending YourTango D6 users');
    $source_fields = array(
      'uid' => t('User ID'),
      'd6_uid' => t('D6 User ID'),
      'roles' => t('The set of roles assigned to a user.'),
      'url_alias' => t('The url alias of the node'),
      'picture_filepath' => t('User Picture filepath'),
      'validity_date' => t('SP Validate Date'),
    );

    $connection = yt_migration_db_connection();
    $query = $connection->select('users', 'u')
        ->fields('u', array('uid', 'name', 'pass', 'mail', 'created', 'access', 'login', 'status', 'init', 'picture'))
        ->condition('u.uid', '127328', '>');

    if (module_exists('pm_email_notify')) {
      $query->leftJoin('pm_email_notify', 'pm', 'pm.user_id = u.uid');
      $query->addField('pm', 'email_notify_is_enabled');
    }

    $this->source = new MigrateSourceSQL($query, $source_fields);
    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    $this->map = new MigrateSQLMap($this->machineName, array(
      'uid' => array(
		      'type' => 'int',
		      'unsigned' => TRUE,
		      'not null' => TRUE,
		      'description' => 'D6 Unique User ID',
      	)
      ), MigrateDestinationUser::getKeySchema()
    );

    // Make the mappings
    //$this->addFieldMapping('uid', 'd6_uid')->dedupe('users', 'uid');
    $this->addFieldMapping('name', 'name')->dedupe('users', 'name');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('mail', 'mail')->dedupe('users', 'mail');
    $this->addFieldMapping('language')->defaultValue(LANGUAGE_NONE);
    $this->addFieldMapping('signature_format')->defaultValue('filtered_html');
    $this->addFieldMapping('created', 'created');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('access', 'access');
    $this->addFieldMapping('login', 'login');
    $this->addFieldMapping('status', 'status');
    $this->addFieldMapping('picture', 'picture')->sourceMigration('YourTangoPicture');
    $this->addFieldMapping('init', 'init');
    $this->addFieldMapping('timezone')->defaultValue(NULL);
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('field_commerce_sp_validity', 'validity_date')->defaultValue(0);
    $this->addFieldMapping('pathauto')->defaultValue(0);
    $this->addFieldMapping('roles', 'roles');
  }

  public function prepareRow($current_row) {
   	$query = db_select('users', 'u')
      ->fields('u', array('uid'))
      ->condition('u.mail', $current_row->mail, '=');
  	$already_user_exists = $query->execute()->fetch();
		if(is_object($already_user_exists) && isset($already_user_exists->uid)) {
			return FALSE;
		}
    $source_id = $current_row->uid;
    $current_row->d6_uid = $current_row->uid;
    $current_row->validity_date = '';
    $connection = yt_migration_db_connection();
    $query = $connection->select('users_roles', 'r')
        ->fields('r', array('uid', 'rid'))
        ->condition('r.uid', $source_id, '=');
    $results = $query->execute();
    $roles = array('2' => '2');
    foreach ($results as $row) {
      if ($row->rid == '7' || $row->rid == '8' || $row->rid == '9') {
        $current_row->validity_date = '1431717330';
      }
      switch ($row->rid) {
        case '3':
          $roles[DESTINATION_ROLE_ADMIN] = DESTINATION_ROLE_ADMIN; // 3 - admin , 3 - DESTINATION_ROLE_ADMIN
          break;
        case '4':
          $roles[DESTINATION_ROLE_AUTHOR] = DESTINATION_ROLE_AUTHOR; //4 - author , 5 - DESTINATION_ROLE_AUTHOR
          break;
        case '5':
          $roles[DESTINATION_ROLE_EDITOR] = DESTINATION_ROLE_EDITOR; //5 - editor , 7 - DESTINATION_ROLE_EDITOR
          break;
        case '7':
          $roles[DESTINATION_ROLE_YTEXPERT_DIRECTORY] = DESTINATION_ROLE_YTEXPERT_DIRECTORY; //7 - proconnect-basic, 12 - DESTINATION_ROLE_YTEXPERT_DIRECTORY
          break;
        case '8':
          $roles[DESTINATION_ROLE_YTEXPERT_AUTHOR] = DESTINATION_ROLE_YTEXPERT_AUTHOR; //8 - proconnect-advanced, 11 - DESTINATION_ROLE_YTEXPERT_AUTHOR
          break;
        case '9':
          $roles[DESTINATION_ROLE_YTEXPERT_PRO] = DESTINATION_ROLE_YTEXPERT_PRO; //9 - proconnect-organization, 13 - DESTINATION_ROLE_YTEXPERT_PRO
          break;
        case '10':
          $roles[DESTINATION_ROLE_YTEXPERT_ADMIN] = DESTINATION_ROLE_YTEXPERT_ADMIN; //10 - proconnect-admin, 10 - DESTINATION_ROLE_YTEXPERT_ADMIN
          break;
        case '11':
          $roles[DESTINATION_ROLE_BLOGGER] = DESTINATION_ROLE_BLOGGER; //11 - blogger, 6 - DESTINATION_ROLE_BLOGGER
          break;
        case '13':
          $roles[DESTINATION_ROLE_AD_ADMIN] = DESTINATION_ROLE_AD_ADMIN; //13 - ads admin, 4 - DESTINATION_ROLE_AD_ADMIN
          break;
        case '14':
          $roles[DESTINATION_ROLE_PARTNER] = DESTINATION_ROLE_PARTNER; //14 - partner, 9 - DESTINATION_ROLE_PARTNER
          break;
        case '16':
          $roles[DESTINATION_ROLE_G_AUTHOR] = DESTINATION_ROLE_G_AUTHOR; //16 - google author, 8 - DESTINATION_ROLE_G_AUTHOR
          break;
        case '17':
          $roles[DESTINATION_ROLE_EDITOR_ADMIN] = DESTINATION_ROLE_EDITOR_ADMIN; //17 - producer, 15 - DESTINATION_ROLE_EDITOR_ADMIN
          break;
      }
    }
    $current_row->roles = $roles;
    $current_row->url_alias = yt_migration_get_user_alias($current_row->uid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    if (module_exists('pm_email_notify')) {
      yt_migration_pm_email_notify_save($entity, $row);
    }
  }
}

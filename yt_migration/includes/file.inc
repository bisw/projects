<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * File Migration
 */
class YourTangoFilesMigration extends YourTangoMigration {

  protected $baseDir;

  public function __construct() {
    parent::__construct();
    $this->description = t('Import files.');
    $this->base_dir = DRUPAL_ROOT . '/sites/default/d6/files/';
    $this->map = new MigrateSQLMap($this->machineName, array(
      'fid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'Source file ID',
        'alias' => 'f',
      ),
        ), MigrateDestinationFile::getKeySchema()
    );

    $source_fields = array(
      'filepath' => t('The url alias of the node'),
      'fid' => t('Drupal 6 fid'),
    );

    $connection = yt_migration_db_connection();
    $query = $connection->select('files', 'f')
        ->fields('f', array('fid', 'uid', 'filename', 'filepath', 'filemime', 'filesize', 'status', 'timestamp'));
    $query->orderBy('f.timestamp', 'ASC');

    $this->source = new MigrateSourceSQL($query, $source_fields);

    $this->destination = new MigrateDestinationFile('file', 'MigrateFileUri');

    $this->addSimpleMappings(array('fid', 'timestamp', 'uid'));
    // Setup common mappings
    $this->addFieldMapping('destination_dir')
        ->defaultValue('public://');
    $this->addFieldMapping('source_dir')
        ->defaultValue(DRUPAL_ROOT . '/sites/default/d6/files/');
    $this->addFieldMapping('file_replace')
        ->defaultValue(MigrateFile::FILE_EXISTS_REUSE);
    $this->addFieldmapping('preserve_files')
        ->defaultValue(TRUE);
    $this->addUnmigratedSources(array('filename', 'filemime', 'filesize'));
  }

}

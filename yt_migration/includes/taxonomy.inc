<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Migration of Blog vocabulary
 */
class YourTangoTaxonomyBlogMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_BLOG, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'tid' => t('Drupal 6 tid'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_BLOG);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;


    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_BLOG,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of Category vocabulary
 */
class YourTangoTaxonomyCategoryMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_CATEGORY, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_CATEGORY);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_BLOG,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of Love Stage vocabulary
 */
class YourTangoTaxonomyLoveStageMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_LOVE_STAGES, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_LOVE_STAGES);

    //$this->systemOfRecord = Migration::DESTINATION;
    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_LOVE_STAGES,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of user tags vocabulary
 */
class YourTangoTaxonomyUserTagsMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_USER_TAGS, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_USER_TAGS);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_USER_TAGS,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of user tags vocabulary
 */
class YourTangoTaxonomySeriesMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_SERIES, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_SERIES);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_SERIES,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of user tags vocabulary
 */
class YourTangoTaxonomyBlockMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_BLOCK, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_BLOCK);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_BLOCK,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of user tags vocabulary
 */
class YourTangoTaxonomySuperTagsMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_SUPER_TAGS, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_SUPER_TAGS);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_SUPER_TAGS,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

/**
 * Migration of user tags vocabulary
 */
class YourTangoTaxonomySuperDuperTagsMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migrate the category for node works.');
    $this->map = new MigrateSQLMap($this->machineName, array(
      'tid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique Term ID',
        'alias' => 'td',
      )
        ), MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    $connection = yt_migration_db_connection();
    $query = $connection->select('term_data', 'td')
        ->fields('td', array('tid', 'vid', 'name', 'description', 'weight'))
        ->condition('td.vid', SOURCE_VOCABULAY_SUPER_DUPER_TAGS, '=');
    $query->join('term_hierarchy', 'th', 'td.tid = th.tid');
    $query->addField('th', 'parent');
    $query->orderBy('th.parent', 'ASC');

    $source_fields = array(
      'url_alias' => t('The url alias of the node'),
      'metatag_title' => t('Meta Data Page Title field for the node'),
      'metatag_description' => t('Meta Data Description field for the node'),
      'metatag_keywords' => t('Meta Data keyword field for the node'),
      'field_meta_canonical_url' => t('External site link'),
      'field_website_name' => t('Website Name'),
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Set up our destination - term in this case.
    $this->destination = new MigrateDestinationTerm(DESTINATION_VOCABULAY_SUPER_DUPER_TAGS);

    // Simple Mappings : map the fields with the same name
    $this->addSimpleMappings(array('tid', 'name', 'description', 'weight'));

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('format')->defaultValue('plain_text');
    $this->addFieldMapping('parent', 'parent')->sourceMigration($this->getMachineName());

    // Taxonomy Meta Tags
    $this->addFieldMapping('metatag_title', 'metatag_title');
    $this->addFieldMapping('metatag_description', 'metatag_description');
    $this->addFieldMapping('metatag_keywords', 'metatag_keywords');
    $this->addFieldMapping('metatag_canonical', 'field_meta_canonical_url');

    // Pass values from prepareRow()
    $this->addFieldMapping('path', 'url_alias');
    $this->addFieldMapping('pathauto')->defaultValue(0);

    // Unmapped source fields
    $this->addUnmigratedSources(array('vid'));

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('parent_name'));
  }

  public function prepareRow($current_row) {
    if ($current_row->parent == 0) {
      unset($current_row->parent);
    }

    $metatag_keywords = yt_migration_get_metatag_data($current_row->tid, '2');
    $current_row->metatag_title = $metatag_keywords->title;
    $current_row->metatag_description = $metatag_keywords->description;
    $current_row->metatag_keywords = $metatag_keywords->keywords;
    $current_row->field_meta_canonical_url = $metatag_keywords->canonical_url;
    $current_row->field_website_name = $metatag_keywords->website_name;

    /* $query = db_insert('taxonomy_term_data')
      ->fields(array('tid', 'name', 'description', 'weight'))
      ->values(array(
      'tid' => $current_row->tid,
      'name' => $current_row->name,
      'description' => $current_row->description,
      'weight' => $current_row->weight,
      'vid' => DESTINATION_VOCABULAY_SUPER_DUPER_TAGS,
      ));
      $query->execute(); */
    $current_row->url_alias = yt_migration_get_term_alias($current_row->tid);
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    //yt_migration_taxonomy_alias_redirect($entity, $row);
    yt_migration_term_metatag_save($entity, $row);
  }

}

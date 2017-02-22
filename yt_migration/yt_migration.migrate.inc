<?php

/*
 * Define some constants to use in class
 * Migration D6 Vocbulary ids
 */
define('SOURCE_VOCABULAY_LOVE_STAGES', '5');
define('SOURCE_VOCABULAY_CATEGORY', '11');
define('SOURCE_VOCABULAY_BLOG', '1');
define('SOURCE_VOCABULAY_USER_TAGS', '10');
define('SOURCE_VOCABULAY_BLOCK', '58');
define('SOURCE_VOCABULAY_SERIES', '6');
define('SOURCE_VOCABULAY_SUPER_TAGS', '15');
define('SOURCE_VOCABULAY_SUPER_DUPER_TAGS', '14');

/**
 * Migration D7 Vocbulary Name
 */
define('DESTINATION_VOCABULAY_LOVE_STAGES', 'love_stages');
define('DESTINATION_VOCABULAY_CATEGORY', 'category');
define('DESTINATION_VOCABULAY_BLOG', 'blog');
define('DESTINATION_VOCABULAY_USER_TAGS', 'user_tags');
define('DESTINATION_VOCABULAY_BLOCK', 'block');
define('DESTINATION_VOCABULAY_SERIES', 'series');
define('DESTINATION_VOCABULAY_SUPER_TAGS', 'super_tags');
define('DESTINATION_VOCABULAY_SUPER_DUPER_TAGS', 'super_duper_tags');
/**
 * Migration drupal 6 role id
  1 - anonymous user
  2 - authenticated user
  3 - admin , DESTINATION_ROLE_ADMIN
  4 - author , DESTINATION_ROLE_AUTHOR
  5 - editor , DESTINATION_ROLE_EDITOR
  7 - proconnect-basic, DESTINATION_ROLE_YTEXPERT_DIRECTORY
  8 - proconnect-advanced, DESTINATION_ROLE_YTEXPERT_AUTHOR
  9 - proconnect-organization, DESTINATION_ROLE_YTEXPERT_PRO
  10 - proconnect-admin, DESTINATION_ROLE_YTEXPERT_ADMIN
  11 - blogger, DESTINATION_ROLE_BLOGGER
  13 - ads admin, DESTINATION_ROLE_AD_ADMIN
  14 - partner, DESTINATION_ROLE_PARTNER
  15 - verified Community member,
  16 - google author, DESTINATION_ROLE_G_AUTHOR
  17 - producer, DESTINATION_ROLE_EDITOR_ADMIN
 */
define('SOURCE_ROLE_ADMIN', '3');
define('SOURCE_ROLE_AUTHOR', '4');
define('SOURCE_ROLE_EDITOR', '5');
define('SOURCE_ROLE_PROCONNECT_BASIC', '7');
define('SOURCE_ROLE_PROCONNECT_ADV', '8');
define('SOURCE_ROLE_PROCONNECT_ORG', '9');
define('SOURCE_ROLE_PROCONNECT_ADMIN', '10');
define('SOURCE_ROLE_BLOGGER', '11');
define('SOURCE_ROLE_COM_ADMIN', '12'); // Do not migrated
define('SOURCE_ROLE_AD_ADMIN', '13');
define('SOURCE_ROLE_PARTNER', '14');
define('SOURCE_ROLE_COMM_MEMBER', '15'); // Do not migrated
define('SOURCE_ROLE_G_AUTHOR', '16');
define('SOURCE_ROLE_PRODUCER', '17');


/**
 * Migration drupal 7 role id
  1 - anonymous user
  2 - authenticated user
  3 - admin,
  4 - ads admin,
  5 - author
  6 - blogger
  7 - editor
  8 - google author
  9 - partner
  10 - ytexpert admin
  11 - ytexpert author
  12 - ytexpert directory
  13 - ytexpert pro
  14 - Editor admin
  15 - Executive Editor Admin
 */
define('DESTINATION_ROLE_ADMIN', '3');
define('DESTINATION_ROLE_AD_ADMIN', '4');
define('DESTINATION_ROLE_AUTHOR', '5');
define('DESTINATION_ROLE_BLOGGER', '6');
define('DESTINATION_ROLE_EDITOR', '7');
define('DESTINATION_ROLE_G_AUTHOR', '8');
define('DESTINATION_ROLE_PARTNER', '9');
define('DESTINATION_ROLE_YTEXPERT_ADMIN', '10');
define('DESTINATION_ROLE_YTEXPERT_AUTHOR', '11');
define('DESTINATION_ROLE_YTEXPERT_DIRECTORY', '12');
define('DESTINATION_ROLE_YTEXPERT_PRO', '13');
define('DESTINATION_ROLE_EDITOR_ADMIN', '14');
define('DESTINATION_ROLE_EXECUTIVE_EDITOR_ADMIN', '15');
/**
 * @file
 * Examples for migration from d6 sources. To use this example
 * (and to run the corresponding) you must define a connection to an d6 database
 * in your settings.php. E.g.,
 *
 * $conf['d6_db'] = array(
 *   'username' => 'DRUPAL',
 *   'password' => 'DRUPAL',
 *   'db_name' => 'DRUPAL',
 *   'host' => 'localhost',
 * );
 */

/**
 * Define a migration process
 */
abstract class YourTangoMigration extends Migration {

  public function __construct() {
    // Always call the parent constructor first for basic setup
    parent::__construct();

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('kulbhushan', 'kulbhushan@ebizontek.com', t('Developer')),
    );

    // Define the external issue or ticket URL pattern here in the shared
    // class with ':id:' representing the position of the issue number, then add
    // ->issueNumber(1234) to a mapping.
    $this->issuePattern = 'https://github.com/bhuntr/n.bhuntr.com/issues/:id:';
  }

}

/**
 * Registers module as an implement of Migrate-based classes.
 * Implement hook_migrate_api()
 */
function yt_migration_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'YourTangoBlog' => array(
        'class_name' => 'YourTangoBlogMigration'
      ),
      'YourTangoVanityUrl' => array(
        'class_name' => 'YourTangoVanityUrlMigration'
      ),
      'YourTangoPrivateMessage' => array(
        'class_name' => 'YourTangoPrivateMessageMigration'
      ),
      'YourTangoUsers' => array(
        'class_name' => 'YourTangoUsersMigration'
      ),
      'YourTangoPendingAuthors' => array(
        'class_name' => 'YourTangoPendingAuthorsMigration'
      ),      
      'YourTangoPendingUsers' => array(
        'class_name' => 'YourTangoPendingUsersMigration'
      ),      
      'YourTangoResources' => array(
        'class_name' => 'YourTangoResourcesMigration'
      ),
      'YourTangoTaxonomyBlog' => array(
        'class_name' => 'YourTangoTaxonomyBlogMigration'
      ),
      'YourTangoExternalArticle' => array(
        'class_name' => 'YourTangoExternalArticleMigration'
      ),
      'YourTangoTaxonomyCategory' => array(
        'class_name' => 'YourTangoTaxonomyCategoryMigration'
      ),
      'YourTangoTaxonomyUserTags' => array(
        'class_name' => 'YourTangoTaxonomyUserTagsMigration'
      ),
      'YourTangoTaxonomyLoveStage' => array(
        'class_name' => 'YourTangoTaxonomyLoveStageMigration'
      ),
      'YourTangoEmbbedVideoYoutube' => array(
        'class_name' => 'YourTangoEmbbedVideoYoutubeMigration'
      ),
      'YourTangoEmbbedVideoBlipTv' => array(
        'class_name' => 'YourTangoEmbbedVideoBlipTvMigration'
      ),
      'YourTangoTaxonomyBlock' => array(
        'class_name' => 'YourTangoTaxonomyBlockMigration'
      ),
      'YourTangoPartnerLink' => array(
        'class_name' => 'YourTangoPartnerLinkMigration'
      ),
      'YourTangoEmbbedVideo' => array(
        'class_name' => 'YourTangoEmbbedVideoMigration'
      ),
      'YourTangoAuthor' => array(
        'class_name' => 'YourTangoAuthorMigration'
      ),
      'YourTangoRedirect' => array(
        'class_name' => 'YourTangoRedirectMigration'
      ),
      'YourTangoPage' => array(
        'class_name' => 'YourTangoPageMigration'
      ),
      'YourTangoGallery' => array(
        'class_name' => 'YourTangoGalleryMigration'
      ),
      'YourTangoFactOfLove' => array(
        'class_name' => 'YourTangoFactOfLoveMigration'
      ),
      'YourTangoList' => array(
        'class_name' => 'YourTangoListMigration'
      ),
      'YourTangoProConnectSS' => array(
        'class_name' => 'YourTangoProConnectSSMigration'
      ),
      'YourTangoFeature' => array(
        'class_name' => 'YourTangoFeatureMigration'
      ),
      'YourTangoGalleryImage' => array(
        'class_name' => 'YourTangoGalleryImageMigration'
      ),
      'YourTangoTaxonomySeries' => array(
        'class_name' => 'YourTangoTaxonomySeriesMigration'
      ),
      'YourTangoTaxonomySuperTags' => array(
        'class_name' => 'YourTangoTaxonomySuperTagsMigration'
      ),
      'YourTangoTaxonomySuperDuperTags' => array(
        'class_name' => 'YourTangoTaxonomySuperDuperTagsMigration'
      ),
    ),
  );
  return $api;
}

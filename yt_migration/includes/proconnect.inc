<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoProConnectMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();

    $this->description = t('Migration of the work pieces for feature.');

    $this->destination = new MigrateDestinationEntityAPI('profile2', 'author');  // 'author' is the "Machine name" of the profile2-"Profile type"
    $this->systemOfRecord = Migration::DESTINATION;
    // Define the field which normally is a primary key
    $this->map = new MigrateSQLMap($this->machineName, array(
      'uid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique User ID',
        'alias' => 'n',
      )
        ), MigrateDestinationEntityAPI::getKeySchema('profile2', 'author')
    );

    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('node', 'n')
        ->fields('n', array('nid', 'vid', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->condition('n.type', 'proconnect', '=')
        ->condition('n.uid', '0', '>');

    // Join with node revisions table
    $query->join('node_revisions', 'nr', 'n.vid = nr.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_proconnect', 'ct', 'ct.vid = n.vid');

    $query->addField('ct', 'field_office_address_lid');
    $query->addField('ct', 'field_years_in_practice_value');
    $query->addField('ct', 'field_license_owner_value');
    $query->addField('ct', 'field_license_number_value');
    $query->addField('ct', 'field_license_state_value');
    $query->addField('ct', 'field_license_expiration_date_value');
    $query->addField('ct', 'field_accept_credit_cards_value');
    $query->addField('ct', 'field_accept_insurance_value');
    $query->addField('ct', 'field_accept_paypal_value');
    $query->addField('ct', 'field_sliding_scale_value');
    $query->addField('ct', 'field_fee_range_value');
    $query->addField('ct', 'field_free_consultation_value');
    $query->addField('ct', 'field_personal_quote_value');
    $query->addField('ct', 'field_signature_line_value');
    $query->addField('ct', 'field_became_professional_value');
    $query->addField('ct', 'field_subscription_id_value');
    $query->addField('ct', 'field_education_pro_value');
    $query->addField('ct', 'field_alternative_credentials_value');
    $query->addField('ct', 'field_medical_credentials_value');
    $query->addField('ct', 'field_where_practice_value');
    $query->addField('ct', 'field_secondary_address_lid');
    $query->addField('ct', 'field_sexual_orientation_value');
    $query->addField('ct', 'field_have_children_value');
    $query->addField('ct', 'field_specialties_in_qs_value');
    $query->addField('ct', 'field_social_linkedin_value');
    $query->addField('ct', 'field_social_facebook_value');
    $query->addField('ct', 'field_social_myspace_value');
    $query->addField('ct', 'field_social_youtube_value');
    $query->addField('ct', 'field_hide_address_value');
    $query->addField('ct', 'field_processor_value');
    $query->addField('ct', 'field_primary_type_of_pro_value');
    $query->addField('ct', 'field_proconnect_video_value');

    $query->orderBy('n.nid', 'ASC');
    // Create a MigrateSource object, which manages retrieving the input data.

    $source_fields = array(
      'user' => t('The profile owner user object'),
    );

    $this->source = new MigrateSourceSQL($query, $source_fields);


    // Mapping: Assign mappings TO destination fields FROM source fields.
    //$this->addFieldMapping('revision_uid', 'uid');
    //$this->addFieldMapping('uid', 'uid');


    $this->addFieldMapping('user', 'user')->sourceMigration('YourTangoAuthor');
    //
    $this->addFieldMapping('field_license_number', 'field_license_number_value');
    $this->addFieldMapping('field_license_expiration_date', 'field_license_expiration_date_value');
    $this->addFieldMapping('field_license_owner', 'field_license_owner_value');
    $this->addFieldMapping('field_license_state', 'field_license_state_value');
    $this->addFieldMapping('field_sexual_orientation', 'field_sexual_orientation_value');
    $this->addFieldMapping('field_do_you_accept_credit_cards', 'field_accept_credit_cards_value');
    $this->addFieldMapping('field_do_you_accept_insurance', 'field_accept_insurance_value');
    $this->addFieldMapping('field_do_you_accept_paypal', 'field_accept_paypal_value');
    $this->addFieldMapping('field_do_you_have_children', 'field_have_children_value');
    $this->addFieldMapping('field_processor', 'field_processor_value');
    $this->addFieldMapping('field_subscription_id', 'field_subscription_id_value');
    $this->addFieldMapping('field_hide_address', 'field_hide_address_value');
    $this->addFieldMapping('field_primary_type_of_pro', 'field_hide_address_value');
    $this->addFieldMapping('field_social_linkedin', 'field_social_linkedin_value');
    $this->addFieldMapping('field_social_myspace', 'field_social_myspace_value');
    $this->addFieldMapping('field_social_facebook', 'field_social_facebook_value');
    $this->addFieldMapping('field_social_youtube', 'field_social_youtube_value');
    $this->addFieldMapping('field_do_you_offer_a_sliding', 'field_sliding_scale_value');
    //
  }

  public function prepareRow($current_row) {
    // Set the text format for the node.
    $current_row->format = yt_migration_get_text_format($current_row->format);
    if ($current_row->uid > 0) {
      $current_row->user = drupal_json_encode(user_load($current_row->uid));
    }
    else {
      return FALSE;
    }
    //watchdog('Proconnect', print_r($current_row, TRUE));
    return TRUE;
  }

}

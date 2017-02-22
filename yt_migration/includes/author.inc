<?php

require_once DRUPAL_ROOT . '/' . drupal_get_path('module', 'yt_migration') . '/yt_migration.migrate.inc';

/**
 * Migration class to importing blog from d6 to d7.
 */
class YourTangoAuthorMigration extends YourTangoMigration {

  public function __construct() {
    parent::__construct();
    $this->description = t('Migration of the work pieces for feature.');
    $this->dependencies = array('YourTangoUsers');
    $this->destination = new MigrateDestinationProfile2('author');  // 'author' is the "Machine name" of the profile2-"Profile type"
    // Define the field which normally is a primary key
    $this->map = new MigrateSQLMap($this->machineName, array(
      'uid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'D6 Unique User ID',
        'alias' => 'n',
      )
        ), MigrateDestinationProfile2::getKeySchema()
    );

    // Grab data from tables in source database
    $connection = yt_migration_db_connection();
    $query = $connection->select('users', 'u')
        ->fields('n', array('nid', 'vid', 'title', 'uid', 'status', 'created', 'changed', 'comment', 'promote', 'moderate', 'sticky', 'tnid', 'translate'))
        ->condition('n.type', 'author', '=')
        ->condition('u.uid', '0', '>');

    // Join with node revisions table
    //$query->join('node_revisions', 'nr', 'n.vid = nr.vid');
    // Join with node revisions table
    $query->join('node', 'n', 'n.uid = u.uid');

    // Grab other fields which can mapping directly
    $query->leftJoin('content_type_author', 'ct', 'ct.vid = n.vid');

    // Grab other fields which can mapping directly
    $query->leftJoin('location', 'l', 'l.lid = ct.field_address_lid');

    // Join with location table
    $query->leftJoin('location_phone', 'lp', 'lp.lid = ct.field_address_lid');

    // Join with location table
    $query->leftJoin('location_fax', 'lf', 'lf.lid = ct.field_address_lid');

    // Join with profile fields
    $query->addField('ct', 'field_gender_value');
    $query->addField('ct', 'field_love_stage_value');
    $query->addField('ct', 'field_about_me_value');
    $query->addField('ct', 'field_twitter_username_value');
    $query->addField('ct', 'field_motto_value');
    $query->addField('ct', 'field_together_since_value');
    $query->addField('ct', 'field_anniversary_date_value');
    $query->addField('ct', 'field_partner_gender_value');
    $query->addField('ct', 'field_tango_newsletter_value');
    $query->addField('ct', 'field_tango_offers_value');
    $query->addField('ct', 'field_allow_birthdate_value');
    $query->addField('ct', 'field_military_value');
    $query->addField('ct', 'field_have_kids_value');
    $query->addField('ct', 'field_attracted_to_value');
    $query->addField('ct', 'field_no_relationship_value');
    $query->addField('ct', 'field_partner_ethnicity_value');
    $query->addField('ct', 'field_birthdate_value');
    $query->addField('ct', 'field_partner_birthdate_value');
    $query->addField('ct', 'field_religion_value');
    $query->addField('ct', 'field_partner_religion_value');
    $query->addField('ct', 'field_education_value');
    $query->addField('ct', 'field_partner_education_value');
    $query->addField('ct', 'field_partner_military_value');
    $query->addField('ct', 'field_ethnicity_value');
    $query->addField('ct', 'field_relationships_lt_6m_value');
    $query->addField('ct', 'field_relationships_lt_1y_value');
    $query->addField('ct', 'field_relationships_gt_1y_value');
    $query->addField('ct', 'field_bad_breakups_value');
    $query->addField('ct', 'field_ever_divorced_value');
    $query->addField('ct', 'field_ever_widowed_value');
    $query->addField('ct', 'field_ever_married_value');
    $query->addField('ct', 'field_address_lid');
    $query->addField('ct', 'field_website_url');
    $query->addField('ct', 'field_website_title');
    $query->addField('ct', 'field_website_attributes');
    $query->addField('ct', 'field_about_me_format');
    $query->addField('ct', 'field_author_title_value');
    $query->addField('ct', 'field_author_first_name_value');
    $query->addField('ct', 'field_author_last_name_value');
    $query->addField('ct', 'field_image_author_fid');
    $query->addField('ct', 'field_image_author_list');
    $query->addField('ct', 'field_image_author_data');
    $query->addField('ct', 'field_feed_url_author_url');
    $query->addField('ct', 'field_feed_url_author_title');
    $query->addField('ct', 'field_feed_url_author_attributes');
    $query->addField('ct', 'field_more_text_author_value');
    $query->addField('ct', 'field_querystring_author_value');
    $query->addField('ct', 'field_feed_image_author_fid');
    $query->addField('ct', 'field_feed_image_author_list');
    $query->addField('ct', 'field_feed_image_author_data');
    $query->addField('ct', 'field_feed_items_author_value');
    $query->addField('l', 'city');
    $query->addField('l', 'name');
    $query->addField('l', 'street');
    $query->addField('l', 'province');
    $query->addField('l', 'postal_code');
    $query->addField('l', 'country');
    $query->addField('l', 'latitude');
    $query->addField('l', 'longitude');
    $query->addField('l', 'source');
    $query->addField('lp', 'phone');
    $query->addField('lf', 'fax');

    // Add source fields which not queried in $query, will be populated in prepareRow()
    $source_fields = array(
      'love_stages' => t('The taxonomy love stages  field for the node'),
      'field_type_of_pro' => t('The url alias of the node'),
      'field_license_state' => t('License State'),
      'field_license_owner' => t('License Owner'),
      'field_license_number' => t('License Number'),
      'field_license_expiration_date' => t('License Expiration'),
      'field_work_remotely_how' => t('Work Remotely'),
      'field_do_you_accept_credit_cards' => t('Accept Credit Card'),
      'field_do_you_accept_insurance' => t('Accept Insurance'),
      'field_do_you_accept_paypal' => t('Accept Paypal'),
      'field_do_you_offer_a_sliding' => t('Offer Sliding'),
      'field_what_is_your_normal_fee' => t('Normal Fee'),
      'field_receive_free_consultation' => t('Meta Data Description field for the node'),
      'field_personal_quote' => t('Meta Data keyword field for the node'),
      'field_signature_line' => t('Meta Data keyword field for the node'),
      'field_became_professional' => t('The url alias of the node'),
      'field_subscription_id' => t('Meta Data Page Title field for the node'),
      'field_education_pro' => t('Meta Data Description field for the node'),
      'field_alternative_credentials' => t('Meta Data keyword field for the node'),
      'field_medical_credentials' => t('Meta Data keyword field for the node'),
      'field_practice_in' => t('Meta Data keyword field for the node'),
      'field_sexual_orientation' => t('The url alias of the node'),
      'field_do_you_have_children' => t('Meta Data Page Title field for the node'),
      'field_specialties_in_qs' => t('Meta Data Description field for the node'),
      'field_social_linkedin' => t('Meta Data keyword field for the node'),
      'field_social_facebook' => t('Meta Data keyword field for the node'),
      'field_social_myspace' => t('Meta Data keyword field for the node'),
      'field_social_youtube' => t('The url alias of the node'),
      'field_hide_address' => t('Meta Data Page Title field for the node'),
      'field_processor' => t('Meta Data Description field for the node'),
      'field_primary_type_of_pro' => t('Meta Data keyword field for the node'),
      'field_embedded_ndn_video_id' => t('Meta Data keyword field for the node'),
      'field_address_name' => t('Office Location Name'),
      'field_address_street' => t('Office Location Street'),
      'field_address_province' => t('Office Location Province'),
      'field_address_postal_code' => t('Office Postal Code'),
      'field_address_country' => t('Office Country'),
      'field_address_fax' => t('Office Fax'),
      'field_address_phone' => t('Office Phone'),
      'field_address_city' => t('Office City'),
      'field_address_source' => t('Office Source'),
      'field_address_latitude' => t('Office Latitude'),
      'field_address_longitude' => t('Office Longitude'),
      'field_coaching_orientation_value' => t('Helping Orientation'),
      'field_have_pets' => t('Do you have pets'),
      'field_years_in_practice' => t('Years Practice'),
      'field_where_practice_in' => t('Where Practice'),
      'field_more_about_relationship' => t('More about your relationship (check all that apply) '),
      'field_credentials' => t('Credentials'),
      'my_specialties' => t('My Special'),
      'field_languages_you_offer' => t('Languages you offer'),
      'amazon_asin' => t('ISBN'),
      'field_suggested_link_url' => ('Field Suggetion Link URL'),
      'field_suggested_link_title' => ('Field Suggetion Link Title'),
      'field_suggested_link_attributes' => ('Field Suggetion Link attributes'),
      'author_title' => t('Proconnect Title'),
      'field_proconnect_video' => t('Proconnect Video ID'),
      'field_proconnect_ndn_playlist_id_value' => t('Proconnect NDN ID'),
      'field_latlong' => t('field latlong'),
      'field_main_specialties_values' => t('Main Specialties'),
      'field_verified_value' => t('Verified by yourtango'),
    );

    $query->orderBy('n.nid', 'ASC');
    $query->groupBy('n.uid');

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query, $source_fields);

    // Mapping: Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('revision_uid', 'uid');
    $this->addFieldMapping('uid', 'uid');
    //$this->addFieldMapping('title', 'author_title');
    $this->addFieldMapping('field_my_specialties', 'my_specialties')->defaultValue('');
    $this->addFieldMapping('field_credentials', 'field_credentials')->defaultValue('');
    $this->addFieldMapping('field_author_title', 'field_author_title_value')->defaultValue('');
    $this->addFieldMapping('field_no_relationship', 'field_no_relationship_value')->defaultValue('');
    $this->addFieldMapping('field_anniversary_date', 'field_anniversary_date_value')->defaultValue('');
    $this->addFieldMapping('field_together_since', 'field_together_since_value')->defaultValue('');
    $this->addFieldMapping('field_partner_gender', 'field_partner_gender_value')->defaultValue('');
    $this->addFieldMapping('field_partner_ethnicity', 'field_partner_ethnicity_value')->defaultValue('');
    $this->addFieldMapping('field_partner_birthdate', 'field_partner_birthdate_value')->defaultValue('');
    $this->addFieldMapping('field_partner_education', 'field_partner_education_value')->defaultValue('');
    $this->addFieldMapping('field_partner_religion', 'field_partner_religion_value')->defaultValue('');
    $this->addFieldMapping('field_partner_military', 'field_partner_military_value')->defaultValue('');
    $this->addFieldMapping('field_author_first_name', 'field_author_first_name_value')->defaultValue('');
    $this->addFieldMapping('field_author_last_name', 'field_author_last_name_value')->defaultValue('');
    $this->addFieldMapping('field_gender', 'field_gender_value')->defaultValue('');
    $this->addFieldMapping('field_ethnicity', 'field_ethnicity_value')->defaultValue('');
    $this->addFieldMapping('field_birthdate', 'field_birthdate_value')->description(t('Format:') . ' "2525-03-01T00:00:00" ' . t('or') . ' "2525-03-01T00:00:00"');
    $this->addFieldMapping('field_allow_birthdate', 'field_allow_birthdate_value')->defaultValue('');
    $this->addFieldMapping('field_education', 'field_education_value')->defaultValue('');
    $this->addFieldMapping('field_religion', 'field_religion_value')->defaultValue('');
    $this->addFieldMapping('field_military', 'field_military_value')->defaultValue('');
    $this->addFieldMapping('field_have_kids', 'field_have_kids_value')->defaultValue('');
    $this->addFieldMapping('field_twitter_username', 'field_twitter_username_value')->defaultValue('');
    $this->addFieldMapping('field_motto', 'field_motto_value')->defaultValue('');
    $this->addFieldMapping('field_attracted_to', 'field_attracted_to_value')->defaultValue('');
    $this->addFieldMapping('field_about_me', 'field_about_me_value')->defaultValue('');
    $this->addFieldMapping('field_have_pets', 'field_have_pets')->defaultValue('');
    $this->addFieldMapping('field_more_about_relationship', 'field_more_about_relationship');
    $this->addFieldMapping('field_website', 'field_website_url')->defaultValue('');
    $this->addFieldMapping('field_website:title', 'field_website_title')->defaultValue('');
    $this->addFieldMapping('field_website:attributes', 'field_website_attributes')->defaultValue('');
    $this->addFieldMapping('field_suggested_link', 'field_suggested_link_url')->defaultValue('');
    $this->addFieldMapping('field_suggested_link:title', 'field_suggested_link_title')->defaultValue('');
    $this->addFieldMapping('field_suggested_link:attributes', 'field_suggested_link_attributes')->defaultValue('');
    $this->addFieldMapping('field_tango_newsletter', 'field_tango_newsletter_value')->defaultValue('');
    $this->addFieldMapping('field_tango_offers', 'field_tango_offers_value')->defaultValue('');
    $this->addFieldMapping('field_relationships_gt_1y', 'field_relationships_gt_1y_value')->defaultValue('');
    $this->addFieldMapping('field_relationships_lt_1y', 'field_relationships_lt_1y_value')->defaultValue('');
    $this->addFieldMapping('field_relationships_lt_6m', 'field_relationships_lt_6m_value')->defaultValue('');
    $this->addFieldMapping('field_ever_divorced', 'field_ever_divorced_value')->defaultValue('');
    $this->addFieldMapping('field_ever_married', 'field_ever_married_value')->defaultValue('');
    $this->addFieldMapping('field_ever_widowed', 'field_ever_widowed_value')->defaultValue('');
    $this->addFieldMapping('field_bad_breakups', 'field_bad_breakups_value')->defaultValue('');
    $this->addFieldMapping('field_type_of_pro', 'field_type_of_pro')->defaultValue('');
    $this->addFieldMapping('field_license_state', 'field_license_state')->defaultValue('');
    $this->addFieldMapping('field_license_owner', 'field_license_owner')->defaultValue('');
    $this->addFieldMapping('field_license_number', 'field_license_number')->defaultValue('');
    $this->addFieldMapping('field_license_expiration_date', 'field_license_expiration_date')->defaultValue('');
    $this->addFieldMapping('field_work_remotely_how', 'field_work_remotely_how')->defaultValue('');
    $this->addFieldMapping('field_do_you_accept_credit_cards', 'field_do_you_accept_credit_cards')->defaultValue('');
    $this->addFieldMapping('field_do_you_accept_insurance', 'field_do_you_accept_insurance')->defaultValue('');
    $this->addFieldMapping('field_do_you_accept_paypal', 'field_do_you_accept_paypal')->defaultValue('');
    $this->addFieldMapping('field_do_you_offer_a_sliding', 'field_do_you_offer_a_sliding')->defaultValue(''); //;
    $this->addFieldMapping('field_what_is_your_normal_fee', 'field_what_is_your_normal_fee')->defaultValue(''); //;
    $this->addFieldMapping('field_receive_free_consultation', 'field_receive_free_consultation')->defaultValue(''); //;
    $this->addFieldMapping('field_personal_quote', 'field_personal_quote')->defaultValue(''); //field_personal_quote_value;
    $this->addFieldMapping('field_signature_line', 'field_signature_line')->defaultValue(''); //field_signature_line_value;
    $this->addFieldMapping('field_became_professional', 'field_became_professional')->defaultValue(''); //field_became_professional_value;
    $this->addFieldMapping('field_subscription_id', 'field_subscription_id')->defaultValue(''); //field_subscription_id_value;
    $this->addFieldMapping('field_education_pro', 'field_education_pro')->defaultValue(''); //field_education_pro_value;
    $this->addFieldMapping('field_alternative_credentials', 'field_alternative_credentials')->defaultValue(''); //field_alternative_credentials_value;
    $this->addFieldMapping('field_medical_credentials', 'field_medical_credentials')->defaultValue(''); //field_medical_credentials_value;
    $this->addFieldMapping('field_years_in_practice', 'field_years_in_practice')->defaultValue(''); //field_where_practice_value;
    $this->addFieldMapping('field_practice_in', 'field_where_practice_in')->defaultValue(''); //field_where_practice_value;
    $this->addFieldMapping('field_language_you_offer_service', 'field_languages_you_offer')->defaultValue(''); //field_where_practice_value;
    $this->addFieldMapping('field_sexual_orientation', 'field_sexual_orientation')->defaultValue(''); //field_sexual_orientation_value;
    $this->addFieldMapping('field_do_you_have_children', 'field_do_you_have_children')->defaultValue(''); //field_have_children_value;
    $this->addFieldMapping('field_specialties_in_qs', 'field_specialties_in_qs')->defaultValue(''); //field_specialties_in_qs_value;
    $this->addFieldMapping('field_social_linkedin', 'field_social_linkedin')->defaultValue(''); //field_social_linkedin_value;
    $this->addFieldMapping('field_social_facebook', 'field_social_facebook')->defaultValue(''); //field_social_facebook_value;
    $this->addFieldMapping('field_social_myspace', 'field_social_myspace')->defaultValue(''); //field_social_myspace_value;
    $this->addFieldMapping('field_social_youtube', 'field_social_youtube')->defaultValue(''); //field_social_youtube_value;
    $this->addFieldMapping('field_hide_address', 'field_hide_address')->defaultValue(''); //field_hide_address_value;
    $this->addFieldMapping('field_processor', 'field_processor')->defaultValue(''); //field_processor_value;
    $this->addFieldMapping('field_primary_type_of_pro', 'field_primary_type_of_pro')->defaultValue(''); //field_primary_type_of_pro_value;
    $this->addFieldMapping('field_proconnect_video', 'field_proconnect_video');
    $this->addFieldMapping('field_embedded_ndn_video_id', 'field_proconnect_ndn_playlist_id_value')->defaultValue('');
    $this->addFieldMapping('field_helping_orientation', 'field_coaching_orientation_value')->defaultValue('');
    $this->addFieldMapping('field_love_stage', 'love_stages')->sourceMigration('YourTangoTaxonomyLoveStage')->arguments(array('source_type' => 'tid'));
		$this->addFieldMapping('field_amazon_asin', 'amazon_asin');
    $office_address_arguments = array(
      'province' => array('source_field' => 'field_address_province'),
      'city' => array('source_field' => 'field_address_city'),
      'street' => array('source_field' => 'field_address_street'),
      'additional' => array('source_field' => 'field_address_additional'),
      'postal_code' => array('source_field' => 'field_address_postal_code'),
      'fax' => array('source_field' => 'field_address_fax'),
      'phone' => array('source_field' => 'field_address_phone'),
      'latitude' => array('source_field' => 'field_address_latitude'),
      'longitude' => array('source_field' => 'field_address_longitude'),
      'country' => array('source_field' => 'field_address_country'),
      'source' => array('source_field' => 'field_address_source'),
    );
    $this->addFieldMapping('field_office_address', 'field_address_country')->arguments($office_address_arguments);

    $address_arguments = array(
      'province' => array('source_field' => 'province'),
      'city' => array('source_field' => 'city'),
      'street' => array('source_field' => 'street'),
      'additional' => array('source_field' => 'additional'),
      'postal_code' => array('source_field' => 'postal_code'),
      'fax' => array('source_field' => 'fax'),
      'phone' => array('source_field' => 'phone'),
      'latitude' => array('source_field' => 'latitude'),
      'longitude' => array('source_field' => 'longitude'),
      'country' => array('source_field' => 'country'),
      'source' => array('source_field' => 'source'),
    );

    $this->addFieldMapping('field_address', 'country')->arguments($address_arguments);

    $secondary_address_arguments = array(
      'province' => array('source_field' => 'secondary_province'),
      'city' => array('source_field' => 'secondary_city'),
      'street' => array('source_field' => 'secondary_street'),
      'additional' => array('source_field' => 'secondary_additional'),
      'postal_code' => array('source_field' => 'secondary_postal_code'),
      'fax' => array('source_field' => 'secondary_fax'),
      'phone' => array('source_field' => 'secondary_phone'),
      'latitude' => array('source_field' => 'secondary_latitude'),
      'longitude' => array('source_field' => 'secondary_longitude'),
      'country' => array('source_field' => 'secondary_country'),
      'source' => array('source_field' => 'secondary_source'),
    );

    $this->addFieldMapping('field_secondary_address', 'secondary_country')->arguments($secondary_address_arguments);

    $this->addFieldMapping('field_main_specialties', 'field_main_specialties_values')->defaultValue('');
    //$this->addFieldMapping('field_member_of_organizations')->defaultValue('');
    //$this->addFieldMapping('field_profile_image')->defaultValue('');        
    $this->addFieldMapping('field_verified_by_yourtango', 'field_verified_value')->defaultValue('');
    //$this->addFieldMapping('field_downgrade_date')->defaultValue('');
    //$this->addFieldMapping('field_join_date')->defaultValue('');
    //$this->addFieldMapping('field_upgrade_date')->defaultValue('');
  }

  public function prepareRow($current_row) {
    // Set the text format for the node.
    //$current_row->format = yt_migration_get_text_format($current_row->format);
		$current_row->migrate = 1;
    // Set the term for the node.
    $current_row->love_stages[$current_row->field_love_stage_value] = $current_row->field_love_stage_value;

    // ISBN
    $current_row->amazon_asin = yt_migration_get_isbn($current_row->uid);

    // Proconnect information
    $proconnect = yt_migration_get_proconnect_node($current_row->uid);

    // Profile information
    $suggestion_link = yt_migration_get_suggestion_link($current_row->nid);

    $field_value = array();

    if (count($suggestion_link)) {
      foreach ($suggestion_link as $key => $more_sugvalue) {
        if ($more_sugvalue->field_suggested_link_url != '') {
          $field_value_url[$more_sugvalue->delta] = $more_sugvalue->field_suggested_link_url;
          $field_value_title[$more_sugvalue->delta] = $more_sugvalue->field_more_about_relationship_value;
          $field_value_attributes[$more_sugvalue->delta] = $more_sugvalue->field_more_about_relationship_value;
        }
      }
      $current_row->field_suggested_link_url = $field_value_url;
      $current_row->field_suggested_link_title = $field_value_title;
      $current_row->field_suggested_link_attributes = $field_value_attributes;
    }
    $pets = yt_migration_get_author_pets($current_row->nid);
    $field_value = array();
    if (count($pets)) {
      foreach ($pets as $key => $pet_value) {
        if ($pet_value->field_have_pets_value != '') {
          $field_value[$pet_value->field_have_pets_value] = $pet_value->field_have_pets_value;
        }
      }
    }

    $current_row->field_have_pets = $field_value;

    $more_rel = yt_migration_get_author_more_rel_details($current_row->nid);
    $field_value = array();

    if (count($more_rel)) {
      foreach ($more_rel as $key => $more_relvalue) {
        if ($more_relvalue->field_more_about_relationship_value != '') {
          $field_value[$more_relvalue->field_more_about_relationship_value] = $more_relvalue->field_more_about_relationship_value;
        }
      }
    }
    //$current_row->author_title = $current_row->title;
    if (empty($current_row->field_author_first_name_value) && empty($current_row->field_author_last_name_value) && empty($current_row->field_author_title_value)) {
      $current_row->field_author_title_value = $current_row->title;
    }

    $current_row->field_more_about_relationship = $field_value;
    if (count($proconnect) > 0) {
      foreach ($proconnect as $item) {

        $work_remote = array();
        $field_type_pro_value = array();
        $simple_credentials = array();
        $field_coaching_orientation = array();
        $field_specialties = array();
        $languages = array();
        $proconnect_title = $item->title;
        //$current_row->author_title = $proconnect_title;
        $work_remote = explode(',', $item->work_remote);
        foreach ($work_remote as $w => $v) {
          $field_work_remotely_how[$v] = $v;
        }

        $field_type_pro_value = explode(',', $item->type_pro);
        foreach ($field_type_pro_value as $a => $b) {
          $field_type_pro[$b] = $b;
        }

        $simple_credentials = explode(',', $item->simple_credentials);
        foreach ($simple_credentials as $c => $d) {
          $field_credentials[$d] = $d;
        }

        $field_coaching_orientation = explode(',', $item->field_coaching_orientation);
        foreach ($field_coaching_orientation as $e => $f) {
          $field_coaching_orientation_value[$f] = $f;
        }

        $field_specialties = explode(',', $item->field_specialties);
        foreach ($field_specialties as $g => $h) {
          $field_specialties_value[$h] = $h;
        }

        $languages = explode(',', $item->languages);
        foreach ($languages as $i => $j) {
          $languages_value[$j] = $j;
        }

        if (count($field_credentials)) {
          $current_row->field_credentials = $field_credentials;
        }
        if (count($field_type_pro)) {
          $current_row->field_type_of_pro = $field_type_pro;
        }
        $current_row->field_license_state = $item->field_license_state_value;
        $current_row->field_license_owner = $item->field_license_owner_value;
        $current_row->field_license_number = $item->field_license_number_value;
        $current_row->field_license_expiration_date = $item->field_license_expiration_date_value;
        if (count($field_work_remotely_how)) {
          $current_row->field_work_remotely_how = $field_work_remotely_how;
        }
        $current_row->field_do_you_accept_credit_cards = $item->field_accept_credit_cards_value;
        $current_row->field_do_you_accept_insurance = $item->field_accept_insurance_value;
        $current_row->field_do_you_accept_paypal = $item->field_accept_paypal_value;
        $current_row->field_do_you_offer_a_sliding = $item->field_sliding_scale_value;
        $current_row->field_what_is_your_normal_fee = $item->field_fee_range_value;
        $current_row->field_receive_free_consultation = $item->field_free_consultation_value;
        $current_row->field_personal_quote = $item->field_personal_quote_value;
        $current_row->field_signature_line = $item->field_signature_line_value;
        $current_row->field_became_professional = $item->field_became_professional_value;
        $current_row->field_subscription_id = $item->field_subscription_id_value;
        $current_row->field_education_pro = $item->field_education_pro_value;
        $current_row->field_alternative_credentials = $item->field_alternative_credentials_value;
        $current_row->field_medical_credentials = $item->field_medical_credentials_value;
        $current_row->field_where_practice_in = $item->field_where_practice_value;
        $current_row->field_sexual_orientation = $item->field_sexual_orientation_value;
        $current_row->field_do_you_have_children = $item->field_have_children_value;
        $current_row->field_specialties_in_qs = $item->field_specialties_in_qs_value;
        $current_row->field_social_linkedin = $item->field_social_linkedin_value;
        $current_row->field_years_in_practice = $item->field_years_in_practice_value;
        $current_row->field_social_facebook = $item->field_social_facebook_value;
        $current_row->field_social_myspace = $item->field_social_myspace_value;
        $current_row->field_social_youtube = $item->field_social_youtube_value;
        $current_row->field_hide_address = $item->field_hide_address_value;

        $current_row->field_processor = $item->field_processor_value;
        $current_row->field_primary_type_of_pro = $item->field_primary_type_of_pro_value;

        $current_row->field_proconnect_ndn_playlist_id_value = $item->field_ndn_playlist_id_value;
        $current_row->field_proconnect_video = $item->field_proconnect_video_value;

        $current_row->field_address_name = $item->name;
        $current_row->field_address_city = $item->city;
        $current_row->field_address_country = $item->country;
        $current_row->field_address_province = $item->province;
        $current_row->field_address_fax = $item->fax;
        $current_row->field_address_phone = $item->phone;
        $current_row->field_address_postal_code = $item->postal_code;
        $current_row->field_address_latitude = $item->latitude;
        $current_row->field_address_longitude = $item->longitude;
        $current_row->field_address_street = $item->street;
        $current_row->field_address_source = $item->source;

        $current_row->field_verified_value = $item->field_verified_value;
        $current_row->field_main_specialties_values = array();
        $extra_specialties = array();
        $specialties_arr = array();
        $specialties_out = array();
        if (!empty($item->field_specialties_in_qs_value)) {
          //Main Specialities
          $specialties_arr = explode(',', $item->field_specialties_in_qs_value);
          $counter_specialties = 0;
          if (count($specialties_arr)) {
            $option_values = yt_migration_get_main_specialities();
            foreach ($specialties_arr as $specialty) {
              if ($specialty) {
                if ($counter_specialties < 3) {
                  if (is_numeric($specialty)) {
                    $counter_specialties = $counter_specialties + 1;
                    $specialties_out[$specialty] = $specialty;
                  }
                  else {
                    $key = array_search($specialty, $option_values);
                    if ($key > 0) {
                      $counter_specialties = $counter_specialties + 1;
                      $specialties_out[$key] = $key;
                    }
                  }
                }
                else {
                  if (is_numeric($specialty)) {
                    $extra_specialties[$specialty] = $specialty;
                  }
                  else {
                    $key = array_search($specialty, $option_values);
                    if ($key > 0) {
                      $extra_specialties[$key] = $key;
                    }
                  }
                }
              }
            }
            $current_row->field_main_specialties_values = $specialties_out;
          }
        }

        if (count($field_coaching_orientation_value)) {
          $current_row->field_coaching_orientation_value = $field_coaching_orientation_value;
        }
        if (count($field_specialties_value)) {
          $current_row->my_specialties = $field_specialties_value;
        }
        if (count($extra_specialties) > 0) {
          foreach ($extra_specialties as $specialties_key => $specialties_value) {
            $current_row->my_specialties[$specialties_key] = $specialties_value;
          }
        }
        if (count($languages_value)) {
          $current_row->field_languages_you_offer = $languages_value;
        }
      }
    }
    return TRUE;
  }

  /**
   * Our post processing method
   */
  public function complete($entity, $row) {
    db_update('profile')
        ->fields(array(
          'created' => $row->created,
          'changed' => $row->changed,
        ))
        ->condition('pid', $entity->pid, '=')
        ->execute();
  }

}

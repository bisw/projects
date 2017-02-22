<?php

/**
 * @file
 * Defines additional tokens for order email.
 */

/**
 * Implements hook_token_info().
 */
function findit_reply_token_info() {
  $data = array();
  $data['types']['review'] = array(
    'name' => t('Reviews and Rating'),
    'description' => t('Tokens related to individual reviews.'),
    'needs-data' => 'review',
  );
  $data['types']['review-extra'] = array(
    'name' => t('Reviews Extra and Mails'),
    'description' => t('Tokens related to mail and extra reviews data.'),
  );
  $data['types']['miscellaneous-mail'] = array(
    'name' => t('This token contail miscellaneous mail'),
    'description' => t('Tokens related to mail.'),
  );
  $data['tokens']['review']['status'] = array(
    'name' => t('Review status'),
    'description' => t('Status of Review.'),
  );
  $data['tokens']['review']['seller_name'] = array(
    'name' => t('Reviews Seller Name'),
    'description' => t('seller on the Review.'),
  );
  $data['tokens']['review']['id'] = array(
    'name' => t('Review Delete status'),
    'description' => t('Delete status of Review.'),
  );
  $data['tokens']['review']['id'] = array(
    'name' => t('Review id'),
    'description' => t('id of Review.'),
  );
  $data['tokens']['review']['entity_id'] = array(
    'name' => t('Review entity_id'),
    'description' => t('entity_id of Review.'),
  );
  $data['tokens']['review']['created'] = array(
    'name' => t('Review Created date'),
    'description' => t('Created date of Review.'),
  );
  $data['tokens']['review']['changed'] = array(
    'name' => t('Review last changed date'),
    'description' => t('last changed date of Review.'),
  );
  $data['tokens']['review']['type'] = array(
    'name' => t('Review type'),
    'description' => t('type of Review.'),
  );
  $data['tokens']['review']['uid'] = array(
    'name' => t("Review author's uid"),
    'description' => t('uid of Review.'),
  );
  $data['tokens']['review']['name'] = array(
    'name' => t("Review author's name"),
    'description' => t("Author's name of Review."),
  );
  $data['tokens']['review']['picture'] = array(
    'name' => t("Review author\s picture"),
    'description' => t("Author's picture of Review."),
  );
  $data['tokens']['review']['description'] = array(
    'name' => t('Review'),
    'description' => t('Body of Review.'),
  );
  $data['tokens']['review']['subject'] = array(
    'name' => t('Reviews Subject'),
    'description' => t('Subject of Review.'),
  );
  $data['tokens']['review']['rating'] = array(
    'name' => t('Reviews Rating'),
    'description' => t('Rating of Review.'),
  );
  $data['tokens']['review']['reviewed_product_title'] = array(
    'name' => t('Reviewed Product Title'),
    'description' => t('Reviewed product title.'),
  );
  $data['tokens']['review']['parent_title'] = array(
    'name' => t('Reviews parent_title'),
    'description' => t('parent_title of Review.'),
  );
  $data['tokens']['review']['parent_url'] = array(
    'name' => t('Reviews parent_url'),
    'description' => t('parent_url of Review.'),
  );
  $data['tokens']['review']['parent_desc'] = array(
    'name' => t('Reviews parent description'),
    'description' => t('parent description of Review.'),
  );
  $data['tokens']['review']['parent_image'] = array(
    'name' => t('Reviews parent image'),
    'description' => t('parent image of Review.'),
  );

  // Merchant review link.
  $data['tokens']['review']['merchant_review_page_seller'] = array(
    'name' => t('Seller Review link Merchant'),
    'description' => t('Merchant Seller Review link Page'),
  );

  $data['tokens']['review']['sku'] = array(
    'name' => t('Reviews product SKU'),
    'description' => t('sku of Review.'),
  );
  $data['tokens']['review']['store_id'] = array(
    'name' => t('Reviews store id'),
    'description' => t('store id of Review.'),
  );
  $data['tokens']['review']['image'] = array(
    'name' => t('Reviews attached image'),
    'description' => t('1st attached image of Review.'),
  );
  $data['tokens']['review']['like'] = array(
    'name' => t('Reviews total likes'),
    'description' => t('Reviews total likes'),
  );
  $data['tokens']['review']['unlike'] = array(
    'name' => t('Reviews total unlikes'),
    'description' => t('Reviews total unlikes'),
  );
  $data['tokens']['review']['report'] = array(
    'name' => t('Reviews total reports as abuse'),
    'description' => t('Reviews total reports as abuse'),
  );
  $data['tokens']['review']['report_reason'] = array(
    'name' => t('Reviews Report reason'),
    'description' => t('Reviews Report reason'),
  );
  $data['tokens']['review']['reason'] = array(
    'name' => t('Reason for review unpublish'),
    'description' => t('Reviews total reports as abuse'),
  );
  // Token added for detecting review/comment.
  $data['tokens']['review']['message_type'] = array(
    'name' => t('Type of message (for comment/review)'),
    'description' => t('Review notification message or comment notification message'),
  );
  // Token added to detect user type as seller/customer.
  $data['tokens']['review']['user_type'] = array(
    'name' => t('User Type (for comment/review)'),
    'description' => t('User Type whether comment/review by Seller or Customer'),
  );
  // Token for getting order summary .
  $data['tokens']['review']['order_summary'] = array(
    'name' => t('Summary of order'),
    'description' => t('Order summary for seller review'),
  );
  // Token for getting order reference number  .
  $data['tokens']['review']['order_reference'] = array(
    'name' => t('Order Reference Number'),
    'description' => t('Order Reference Number for seller review'),
  );
  // Token for review author in case of comment by other user.
  $data['tokens']['review']['parent_author'] = array(
    'name' => t('Author in case of comment by other user(other than author of review)'),
    'description' => t('Author in case of comment ,if review wae written by other user and comment is done by another user(Note:Author is the user who wrote comment)'),
  );
  $data['tokens']['review']['review_page_url'] = array(
    'name' => t('Review page url for user'),
    'description' => t('Review page url for user'),
  );
  $data['tokens']['review']['review_url'] = array(
    'name' => t('Review page url'),
    'description' => t('Review page url'),
  );
  $data['tokens']['miscellaneous-mail']['findit-reason-for-blocking'] = array(
    'name' => t('findit-reason-for-blocking'),
    'description' => t('findit Reason for blocking user'),
  );
  $data['tokens']['miscellaneous-mail']['findit-reason-for-unblocking'] = array(
    'name' => t('findit-reason-for-unblocking'),
    'description' => t('findit Reason for unblocking user'),
  );
  $data['tokens']['review-extra']['findit-reply-customer-name'] = array(
    'name' => t('findit-reply-customer-name'),
    'description' => t('findit reply customer name'),
  );
  $data['tokens']['review-extra']['findit-customer-reply-desc'] = array(
    'name' => t('findit-customer-reply-desc'),
    'description' => t('findit reply customer description'),
  );
  $data['tokens']['review-extra']['findit-reply-product-title'] = array(
    'name' => t('findit-reply-product-title'),
    'description' => t('findit reply product title'),
  );
  $data['tokens']['review-extra']['findit-reply-product-path'] = array(
    'name' => t('findit-reply-product-path'),
    'description' => t('findit reply product path'),
  );
  $data['tokens']['review-extra']['findit-reply-product-image'] = array(
    'name' => t('findit-reply-product-image'),
    'description' => t('findit reply product image'),
  );
  $data['tokens']['review-extra']['findit-reply-rating'] = array(
    'name' => t('findit-reply-rating'),
    'description' => t('findit reply rating'),
  );
  $data['tokens']['review']['raise-to-findit-reason'] = array(
    'name' => t('raise-to-findit-reason'),
    'description' => t('findit reply rating'),
  );

  return $data;
}

/**
 * Implements hook_token_info_alter().
 */
function findit_reply_token_info_alter(&$data) {
  $data['tokens']['site']['commerce-marketplace-link'] = array(
    'name' => t('Marketplace url'),
    'description' => t('Marketplace url.'),
  );
}

/**
 * Implements hook_tokens().
 */
function findit_reply_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $replacements = array();
  if ($type == 'site') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'commerce-marketplace-link':
          $replacements[$original] = variable_get('marketplace_url', '');
          break;
      }
    }
  }

  if ($type == 'review-extra') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'findit-reply-customer-name':
          $replacements[$original] = $data['review-extra']->fullname;
          break;

        case 'findit-customer-reply-desc':
          $replacements[$original] = $data['review-extra']->review_desc;
          break;

        case 'findit-reply-product-title':
          $product_path = $data['review-extra']->product_path;
          $title = '<a href="' . $product_path . '">' . $data['review-extra']->product_title . '</a>';
          $replacements[$original] = $title;
          break;

        case 'findit-reply-product-path':
          $replacements[$original] = $data['review-extra']->product_path;
          break;

        case 'findit-reply-product-image':
          $product_sku = $data['review-extra']->sku;
          $product_image = findit_commerce_product_image($product_sku);
          $replacements[$original] = $product_image;
          break;

        case 'findit-reply-rating':
          $rating = $data['review-extra']->review_rating;
          $replacements[$original] = $rating;
          break;
      }
    }
  }

  if ($type == 'miscellaneous-mail') {
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'findit-reason-for-blocking':
          $replacements[$original] = $data['miscellaneous-mail']->scalar;
          break;

        case 'findit-reason-for-unblocking':
          $replacements[$original] = $data['miscellaneous-mail']->scalar;
          break;
      }
    }
  }

  if ($type == 'review' && !empty($data['review'])) {
    $review = $data['review'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'status':
          $replacements[$original] = $review['status'] == 1 ? t('Publish') : t('Unpublish');
          break;

        case 'deleted':
          $replacements[$original] = $review['deleted'] == 1 ? t('Deleted') : t('Not deleted');
          break;

        case 'id':
          $replacements[$original] = $review['id'];
          break;

        case 'entity_id':
          $replacements[$original] = $review['entity_id'];
          break;

        case 'created':
          $replacements[$original] = format_date($review['created']);
          break;

        case 'changed':
          $replacements[$original] = format_date($review['changed']);
          break;

        case 'seller_name':
          $replacements[$original] = $review['seller_name'];
          break;

        case 'type':
          $replacements[$original] = isset($review['type']) ? $review['type'] : '';
          break;

        case 'uid':
          $replacements[$original] = $review['uid'];
          break;

        case 'name':
          $replacements[$original] = isset($review['name']) ? $review['name'] : '';
          break;

        case 'picture':
          $replacements[$original] = $review['image'];
          break;

        case 'description':
          $replacements[$original] = isset($review['description']) ? $review['description'] : '';
          break;

        case 'subject':
          $replacements[$original] = isset($review['subject']) ? $review['subject'] : '';
          break;

        case 'rating':
          $replacements[$original] = $review['rating'] / 20;
          break;

        case 'reviewed_product_title':
          $product_title_info = _fetch_reviewed_product_title_on_product($review['sku']);
          $replacements[$original] = ($review['type'] == 'product') ? $product_title_info : '';
          break;

        case 'parent_title':
          $replacements[$original] = isset($review['parent_title']) ? $review['parent_title'] : '';
          break;

        case 'parent_url':
          $replacements[$original] = $review['parent_url'];
          break;

        case 'parent_image':
          if (isset($review['parent_image'])) {
            $replacements[$original] = '<img src="' . $review['parent_image'] . ' " alt="' . $review['parent_title'] . '"/>';
          }
          else {
            $replacements[$original] = '';
          }
          break;

        case 'parent_desc':
          $replacements[$original] = $review['parent_desc'];
          break;

        case 'sku':
          $replacements[$original] = $review['sku'];
          break;

        case 'store_id':
          $replacements[$original] = isset($review['store_id']) ? $review['store_id'] : '';
          break;

        case 'image':
          $replacements[$original] = isset($review['attacted_images']) && isset($review['attacted_images'][0]) ? $review['attacted_images'][0]['image'] : '';
          break;

        case 'like':
          $replacements[$original] = $review['like'];
          break;

        case 'unlike':
          $replacements[$original] = $review['dislike'];
          break;

        case 'report':
          $replacements[$original] = $review['report'];
          break;

        case 'report_reason':
          $replacements[$original] = (isset($review['report_reason']) && !empty($review['report_reason'])) ? implode('<br/>', $review['report_reason']) : 'N/A';
          break;

        case 'reason':
          $replacements[$original] = (isset($review['reason']) && $review['status'] == 0) ? ($review['reason'] ? $review['reason'] : 'N/A') : 'N/A';
          break;

        case 'review_page_url':
          $replacements[$original] = (isset($review['review_page_url'])) ? $review['review_page_url'] : $review['parent_url'];
          break;

        case 'review_url':
          $replacements[$original] = ($review['type'] == 'product') ? variable_get('marketplace_url', '') . '/product/user/review/' . $review['id'] : variable_get('marketplace_url', '') . '/product/seller/review/' . $review['id'];
          break;

        case 'message_type':
          $replacements[$original] = findit_get_msg_type($review['parent']);
          break;

        case 'user_type':
          $replacements[$original] = findit_get_usertype($review[uid]);
          break;

        case 'order_summary':
          $order_items = (isset($review['order_ref'])) ? findit_commerce_tokens_order_items($review['order_ref']) : ' ';
          $replacements[$original] = $order_items;
          break;

        case 'order_reference':
          $replacements[$original] = (isset($review['order_id']) && isset($review['sku'])) ? $review['order_id'] . '-' . $review['sku'] : 'N/A';
          break;

        case 'parent_author':
          $author_name = (isset($data['user'])) ? $data['user']->field_user_first_name[LANGUAGE_NONE][0]['value'] : 'Author/Customer';
          $replacements[$original] = $author_name;
          break;

        case 'merchant_review_page_seller':
          global $base_url;
          $storeid = findit_get_store_id($review['store_id']);
          $replacements[$original] = $base_url . '/store/' . $storeid . '/review';
          break;

        case 'raise-to-findit-reason':
          if ($data['review']['raised_to_findit_reason'] != 'other') {
            $reason_trimmed = trim($data['review']['raised_to_findit_reason']);
            $replacements[$original] = str_replace('_', ' ', $reason_trimmed);
          }
          else {
            $replacements[$original] = $data['review']['raised_to_findit_reason_other'];
          }
          break;
      }
    }
  }
  return $replacements;
}

/**
 * Function for getting store id from unique store id.
 *
 * @param string $store_uniq_id
 *   Unique store id.
 *
 * @return int
 *   Store id.
 */
function findit_get_store_id($store_uniq_id) {
  $result = db_select('commerce_store', 'cs')
      ->fields('cs', array('store_id'))
      ->condition('cs.unique_store_id', $store_uniq_id)
      ->execute()
      ->fetchField();
  return $result;
}

/**
 * Function for setting product image properties.
 *
 * @param string $product_sku
 *   Sku of the product.
 *
 * @return array
 *   Array suitable for use by theme('table' ...)
 */
function findit_commerce_product_image($product_sku) {
  $product_obj = commerce_product_load_by_sku($product_sku);
  $product_id = $product_obj->product_id;
  $product_title = $product_obj->title;
  $title = htmlentities($product_title, ENT_QUOTES, "UTF-8");
  $attr = findit_commerce_tokens_order_item_attributes($product_id);
  if ($attr['sku'] != '') {
    $title .= ' (' . $attr['sku'] . ')';
  }
  if ($attr['detail'] != '') {
    $title .= '<br /><em>' . $attr['detail'] . '</em>';
  }
  $rows[] = array(
    'data' => array(
      $attr['image'],
      array('data' => $title, 'style' => array('text-align: left;')),
    ),
  );
  $table = array(
    'header' => array(),
    'rows' => $rows,
    'attributes' => array('style' => array('width: 100%; border: 1px solid #ddd;')),
  );

  $output .= theme('table', $table);
  return $output;
}

/**
 * Function for seller review information.
 */
function findit_commerce_seller_review_info($data) {
  $review_subject = $data['review-extra-on-seller']->review_subject;
  $review_desc = $data['review-extra-on-seller']->review_desc;

  $rows[] = array(
    'data' => array(
      0 => $review_subject,
      1 => $review_desc,
      'style' => array('text-align: left;'),
    ),
  );
  $table = array(
    'header' => array(),
    'rows' => $rows,
    'attributes' => array('style' => array('width: 100%; border: 1px solid #ddd;')),
  );
  $output .= theme('table', $table);
  return $output;
}

/**
 * Function for order summary in table format.
 *
 * @param object $data
 *   Order related data.
 *
 * @return array
 *   Array suitable for use by theme('table' ...)
 */
function findit_commerce_seller_order_summary_info($data) {
  $order_ref = $data['review-extra-on-seller']->order_reference;
  $order_detail = explode('--', $order_ref);
  $order_reference_id = $order_detail[0];
  $order_id = _fetch_order_id_from_order_ref($order_reference_id);
  $order = commerce_order_load($order_id);
  $wrapper = entity_metadata_wrapper('commerce_order', $order);
  $table = findit_commerce_tokens_prepare_table($wrapper, 'commerce_order');
  return theme('table', $table);
}

/**
 * Function for getting seller page URL.
 *
 * @param array $data
 *   Store related data.
 *
 * @return string
 *   Seller page url.
 */
function findit_commerce_seller_profile_page($data) {
  $market_base_url = variable_get('marketplace_url', '');
  $store_id = $data['review-extra-on-seller']->store_unique_id;
  $seller_page_url = $market_base_url . '/seller/' . $store_id . '/profile';
  return $seller_page_url;
}

/**
 * Function for getting order id from order reference ID.
 */
function _fetch_order_id_from_order_ref($order_reference_id) {
  $query = db_select('field_data_field_order_refrence', 'fdfor')
      ->fields('fdfor', array('entity_id'))
      ->condition('field_order_refrence_value', $order_reference_id, '=')
      ->execute();
  $order_data = $query->fetchCol();
  $order_id = $order_data[0];
  return $order_id;
}

/**
 * Function for getting seller name fom unique store id in review info.
 */
function findit_seller_name($data) {
  $store_unique_id = $data['review-extra-on-seller']->store_unique_id;
  $query = db_select('commerce_store', 'cs')
      ->fields('cs', array('title'))
      ->condition('unique_store_id', $store_unique_id, '=')
      ->execute();
  $seller_name = $query->fetchField();
  return $seller_name;
}

/**
 * Function for getting product title from SKU.
 */
function _fetch_reviewed_product_title_on_product($sku) {
  $product_obj = commerce_product_load_by_sku($sku);
  $product_title = $product_obj->title;
  return $product_title;
}

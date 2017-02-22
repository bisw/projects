<?php

/**
 * @file
 * Class related to order processing.
 */
class finditreply {

  /**
   * Implements ServicesResourceControllerInterface::access().
   */
  public static function access($op, $args) {
    return TRUE;
    if ($op == 'view') {
      return TRUE;
    }
    if ($op == 'index') {
      // Access is handled per-entity by index().
      return user_access('view review at merchant site');
    }
    // For create operations, we need to pass a new entity to entity_access()
    // in order to check per-bundle creation rights. For all other operations
    // we load the existing entity instead.
    if ($op == 'create') {
      return user_access('view review at merchant site');
    }
    else {
      return user_access('view review at merchant site');
    }
  }

  /**
   * Function for getting metadata related to review.
   */
  public static function get_metadata($values = array()) {
    global $user;
    $role = findit_reply_user_get_role();
    $return = array(
      'role' => $role,
      'uid' => $user->uid,
      'name' => $user->name,
      'site' => variable_get('site_name', "Merchant App"),
    );
    if (isset($values['action'])) {
      $return['action'] = $values['action'];
    }
    return $return;
  }

  /**
   * Implements ServicesResourceControllerInterface::create().
   */
  public static function create(array $values) {

    $values['metadata'] = self::get_metadata();
    return self::get_result_from_marketplace('create', array('data' => $values));
  }

  /**
   * Implements ServicesResourceControllerInterface::retrieve().
   */
  public static function retrieve($entity_id, $fields = '*', $revision = 0, $store = NULL) {
    if ($store != NULL) {
      return self::get_result_from_marketplace('retrieve', array(
            'entity_id' => $entity_id,
            'data' => array(
              'store_id' => $store,
              'fields' => $fields,
              'revision' => $revision,
            ),
              )
      );
    }
    return self::get_result_from_marketplace('retrieve', array(
          'entity_id' => $entity_id,
          'data' => array('fields' => $fields, 'revision' => $revision),
            )
    );
  }

  /**
   * Implements ServicesResourceControllerInterface::update().
   */
  public static function update($entity_id, array $values) {
    $values['metadata'] = self::get_metadata($values);
    return self::get_result_from_marketplace('update', array('entity_id' => $entity_id, 'data' => $values));
  }

  /**
   * Implements ServicesResourceControllerInterface::delete().
   */
  public static function delete($entity_id, array $reply) {
    return self::get_result_from_marketplace('delete', array('entity_id' => $entity_id, 'data' => $reply));
  }

  /**
   * Implements ServicesResourceControllerInterface::index().
   */
  public static function index($fields, $parameters = array(), $page = 0, $pagesize = 2, $sort = 'created', $direction = 'DESC') {
    if ($sort == '') {
      $sort = 'created';
      $direction = 'DESC';
    }

    return self::get_result_from_marketplace('index', array(
          'data' => array(
            'fields' => $fields,
            'parameters' => $parameters,
            'page' => $page,
            'pagesize' => $pagesize,
            'sort' => $sort,
            'direction' => $direction,
          ),
            )
    );
  }

  /**
   * Function for making API request from marketplace.
   */
  protected static function get_result_from_marketplace($op = 'index', $var = array()) {
    return self::httpRequest($op, $var, array('Content-Type' => 'application/json'));
  }

  /**
   * Makes a HTTP request.
   */
  public static function httpRequest($op = 'index', $data = NULL, $headers = array()) {
    static $user_sessions = array();
    $url = variable_get('marketplace_reply_url', '');

    if (empty($user_sessions)) {
      $username = variable_get('marketplace_reply_user', '');
      $pass = variable_get('marketplace_reply_pass', '');

      $options = array(
        'method' => 'POST',
        'headers' => array('Content-Type' => 'application/json'),
        'data' => drupal_json_encode(array(
          'username' => variable_get('marketplace_reply_user', ''),
          'password' => variable_get('marketplace_reply_pass', ''),
        )),
      );
      $login_url = $url . '/user/login';
      $response = drupal_http_request($login_url, $options);

      if (isset($response->error) || !in_array($response->code, array(
            200,
            304,
              )
          )
      ) {
        watchdog('review', 'Authentication failed: <pre>@response</pre>', array('@response' => print_r($response, TRUE)), WATCHDOG_DEBUG);
        return array();
      }
      $user_data = drupal_json_decode($response->data);
      $user_sessions = $user_data;
    }
    else {
      $user_data = $user_sessions;
    }
    $options = array(
      'headers' => array('Content-Type' => 'application/json'),
      'data' => drupal_json_encode($data['data']),
    );

    $options['headers']['Cookie'] = $user_data['session_name'] . "=" . $user_data['sessid'] . ";";

    switch ($op) {
      case 'index':
        $url = $url . '/reply';
        $url .= '?' . drupal_http_build_query($data['data']);
        $method = 'GET';
        break;

      case 'retrieve':
        $url = $url . '/reply/' . $data['entity_id'];
        $url .= '?' . drupal_http_build_query($data['data']);
        $method = 'GET';
        break;

      case 'update':
        $url = $url . '/reply/' . $data['entity_id'];
        $method = 'PUT';
        break;

      case 'create':
        $url = $url . '/reply';
        $method = 'POST';
        break;

      case 'delete':
        $url = $url . '/reply/' . $data['entity_id'];
        $method = 'DELETE';
        break;

      default:
        $url = $url . '/reply&' . drupal_http_build_query($data);
        $method = 'GET';
        break;
    }
    if ($method != 'GET') {
      $options['method'] = $method;
      $options['headers']['X-CSRF-TOKEN'] = $user_data['token'];
    }
    // if (isset($_REQUEST['store_id'])) {
    // $url .= '&store_id=' .  self::commerce_order_get_store_unique_id($_REQUEST['store_id']);
    // }
    $response = drupal_http_request($url, $options);
    if (isset($response->error) || !in_array($response->code, array(200, 304))) {
      watchdog('review', 'Review service error: <pre>@response</pre>', array('@response' => print_r($response, TRUE)), WATCHDOG_DEBUG);
      return array();
    }
    return drupal_json_decode($response->data);
  }

  /**
   * Function for getting unique store id from store id.
   */
  public static function commerce_order_get_store_unique_id($store_id) {
    $result = db_select('commerce_store', 'cs')
        ->fields('cs', array('unique_store_id'))
        ->condition('cs.store_id', $store_id)
        ->execute()
        ->fetchField();
    return $result;
  }

  /**
   * Function for sending mail on review ops.
   */
  public static function mail($method) {
    $data = drupal_json_decode($method['data']);
    $dummy_id = variable_get('admin_dummy_id');
    $review = $data['review'];
    if ($data['action'] == 'update') {
      if ($review['status'] != $review['original']['status'] && $review['deleted'] == $review['original']['deleted'] && $review['type'] == 'store') {
        // Check if admin's own comment is published.
        if ($review['uid'] != $dummy_id) {
          // Check if comment published is by thread author.
          if ($data['account']['mail'] != $data['author_account']['mail'] && $review['parent'] != 0 && $review['status'] == 1 && $review['original']['status'] == 0) {
            findit_reply_send_mail_review_author($review, 'status', (object) $data['author_account']);
          }

          if ($review['status'] == 1 && $review['original']['status'] == 0) {
            findit_reply_send_mail_customer_review($review, 'publish', (object) $data['account']);
          }
          else {
            findit_reply_send_mail_customer_review($review, 'unpublish', (object) $data['account']);
          }
          if ($review['parent'] == 0) {
            findit_reply_send_mail_merchant_comment_status($review);
          }
          elseif ($review['parent'] != 0 && $review['status'] == 1 && $review['original']['status'] == 0) {
            findit_reply_send_mail_merchant_comment_status($review);
          }
        }
        else {
          $store = findit_reply_order_get_store_unique_id($review['store_id']);
          // To prevent empty token mail.
          if ($review['description'] != "" || $review['type'] != "") {
            $user = user_load($store->uid);
            if ($review['status'] == 1 && $review['original']['status'] == 0) {
              findit_reply_send_mail_review_author($review, 'status', (object) $data['author_account']);
              findit_reply_send_mail_customer_review($review, 'publish', $user);
            }
            else {
              findit_reply_send_mail_customer_review($review, 'unpublish', $user);
            }
          }
        }
      }

      if ($review['type'] == 'product' && $review['status'] != $review['original']['status'] && $review['deleted'] == $review['original']['deleted']) {
        if ($review['status'] == 1 && $review['original']['status'] == 0) {
          findit_reply_send_mail_customer_review($review, 'publish', (object) $data['account']);
          if ($review['parent'] == 0) {
            findit_reply_send_mail_merchant_product_review($review);
          }
        }
        else {
          findit_reply_send_mail_customer_review($review, 'unpublish', (object) $data['account']);
        }

        if ($data['account']['mail'] != $data['author_account']['mail'] && $review['parent'] != 0 && $review['status'] == 1 && $review['original']['status'] == 0) {
          findit_reply_send_mail_review_author($review, 'status', (object) $data['author_account']);
        }
      }
      if ($review['deleted'] == 1 && $review['original']['deleted'] == 0) {
        findit_reply_send_mail_customer_review($review, 'deleted', (object) $data['account']);
      }
    }
    if ($data['action'] == 'report') {
      findit_reply_send_mail_customer_review($review, 'report', (object) $data['account']);
    }

    return array(1);
  }

}

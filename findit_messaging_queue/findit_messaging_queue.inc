<?php

/**
 * @file
 */
module_load_include('inc', 'findit_messaging_queue', 'classes/queue_services_connector');
require DRUPAL_ROOT . '/vendor/autoload.php';

use Aws\Sqs\SqsClient;

class MessageSender {

  private $aws_connection;

  public function __construct() {
    $this->aws_connection = SqsClient::factory(array(
          'key' => 'AKIAJRVZPHCE27VANUVQ',
          'secret' => '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',
          'region' => 'ap-southeast-1'
    ));
  }

  public function execute($msg, $routing_key, $operation) {
    if ($routing_key == 'findit_merchant_place_to_market') {
      $queueurl = 'https://sqs.ap-southeast-1.amazonaws.com/572686444491/findit_merchant_place_to_market';
    }
    else {
      $queueurl = 'https://sqs.ap-southeast-1.amazonaws.com/572686444491/findit_audit_log_message';
    }
    $this->aws_connection->sendMessage(array(
      'QueueUrl' => $queueurl,
      'MessageBody' => $msg,
      'DelaySeconds' => 30,
      'MessageAttributes' => array(
        'message_id' => array(
          'DataType' => 'String',
          'StringValue' => $operation
        ),
      ),
    ));
  }

}

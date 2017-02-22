<?php

/**
 * @file 
 * Factory class for other methods
 */
module_load_include('inc','findit_messaging_queue','classes/queue_services_connector');
class FinditFactory {

  /**
   * @param type $classname
   * @param type $methodname
   * $Classname will be classname string and $methodname will be methodname string
   */
  public function __construct($key, $data, $url = NULL) {    
    $QueueServicesConnector = new QueueServicesConnector;
    if (!$url) {
      $url = $QueueServicesConnector->services_get($key);
    }
    drupal_http_request($url, $data);
  }

}

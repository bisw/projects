<?php

/**
 * @file
 * Provides a controller for interactiong with message queue.
 */

/**
 * Interface for crud operations on findit merchant
 */
interface finditQueueInterface {

  /**
   * @param type $data
   * @param type $apikey
   * Callback method to get data from queue and call a rest api.
   */
 public function queuereceivercallback($data, $apikey);

  /**
   * @param type $data
   * @param type $op
   * Method to call queue api.
   */
  public function queueapiinvoke($data, $op);

 /**
  * @param type $data
  * Send data to queue
  */
  public function sendtoqueue($data);

  /**
   * @param type $key
   * Get service url based on key passed.
   */
  public function services_get($key);

  /**
   * Check if a current user has access
   * @param expects logged user id
   * @return True/False
   */
  public function queueapireceiver($data);
  
  /**
   * Check whether current user has access to queue.
   */
  public function queueaccess();
}

<?php

/**
 * @file
 * Provides a controller to perform crud operations using service.
 */

/**
 * Interface for crud operations on findit merchant
 */
interface finditServicesInterface {

  /**
   * Create a new entity
   * 
   * * @param array $values
   *   An array of values to set, keyed by property name.
   * @return entity id
   */
  public function create($values);

  /**
   * Retrieve data from services
   */
  public function retrieve($id);

  /**
   * @param $ids
   *   An array of entity IDs.
   */
  public function update($id,$data);

  /**
   * Delete permanently saved entities.
   *
   * In case of failures, an exception is thrown.
   *
   * @param $ids
   *   An array of entity IDs.
   */
  public function delete();

  /**
   * Check if a current user has access
   * @param expects logged user id
   * @return True/False
   */
  public function access();
}

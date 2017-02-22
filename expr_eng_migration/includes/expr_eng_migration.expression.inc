<?php

function expr_eng_migration_fetch_channels() {
  $recordReturn = array();
  $queryChannel = db_select('exp_channels', 'ch');
  $queryChannel->fields('ch');
  $resultChannel = $queryChannel->execute();

  while($recordChannel = $resultChannel->fetchAssoc()) {
    $recordReturn[] = $recordChannel;
  }
  return $recordReturn;
}

function expr_eng_migration_fetch_field_ids() {
  $fields = array();
  $queryChannel = db_select('exp_channel_fields', 'bf');
  $queryChannel->fields('bf', array('field_id', 'field_name'));
  $resultChannel = $queryChannel->execute(); 

  while($recordChannel = $resultChannel->fetchAssoc()) {
    $fields[$recordChannel['field_id']] = $recordChannel['field_name'];
  }
  return $fields;
}

function expr_eng_migration_fetch_fields() {
  $fields = array();
  $validFieldTypes = array('text', 'textarea');
  $queryChannel = db_select('exp_channel_fields', 'bf');
  $queryChannel->fields('bf');
  $resultChannel = $queryChannel->execute(); 

  while($recordChannel = $resultChannel->fetchAssoc()) {
    if(in_array($recordChannel['field_type'], $validFieldTypes) && ($recordChannel[''])) { 
      switch($recordChannel['field_type']) {
        case 'textarea':
          $type = 'text_long';
          break;
            
        default:
          $type = 'text';
          break;
      }
      $recordChannel['type'] = $type;
      $fields[$recordChannel['field_id']] = $recordChannel;
    }
  }
  return $fields;
}

function expr_eng_migration_get_categories() { 
  $categories = array();
    
  $vocaQuery = db_select('exp_category_groups', 'voca');
  $vocaQuery->fields('voca');
  $vocaResult = $vocaQuery->execute(); 
    
  while($vocaRecord = $vocaResult->fetchAssoc()) {
    $categories[$vocaRecord['group_id']] = $vocaRecord['group_name'];
  }
    
  return $categories;
}

function expr_eng_migration_fetch_categories() { 
  $categories = array();
  $vocaQuery = db_select('exp_category_groups', 'voca');
  $vocaQuery->fields('voca');
  $vocaResult = $vocaQuery->execute(); 
    
  while($vocaRecord = $vocaResult->fetchAssoc()) {
    $categories[] = $vocaRecord;
  }
  return $categories;
}

function expr_eng_migration_fetch_taxonomy_terms() { 
  $terms = array();
  $termQuery = db_select('exp_categories', 'term');
  $termQuery->fields('term');
  $termResult = $termQuery->execute(); 
    
  while($termRecord = $termResult->fetchAssoc()) {
    $terms[] = $termRecord;
  }
  return $terms;
}

function expr_eng_migration_fetch_channels_data() {
  $nodes = array();
  $nodeQuery = db_select('exp_channel_data', 'node');
  $nodeQuery->join('exp_channels', 'ch', 'node.channel_id = ch.channel_id');
  $nodeQuery->join('exp_channel_titles', 'ct', 'node.entry_id = ct.entry_id');
  $nodeQuery->fields('node');
  $nodeQuery->fields('ch', array('channel_name', 'channel_title'));
  $nodeQuery->fields('ct', array('title'));
  $nodeResult = $nodeQuery->execute(); 
    
  while($nodeRecord = $nodeResult->fetchAssoc()) {
    $nodes[] = expr_eng_migration_set_field_value($nodeRecord);
  }
  return $nodes;
}
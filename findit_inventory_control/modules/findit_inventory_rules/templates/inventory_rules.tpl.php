<?php
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
global $base_path;
?>

<h1 class="commision-title">
  <?php print t('Findit Inventory Rules Interface'); ?>
</h1>
<style>
  /* .category-tree{
     float: left;
     width: 35%;
   }*/

  .update-inventory-wrapper{
    width: 80%;
    left: 40%;
    top:300;
  }
  .changes{
    float: right;
  }
  .loader-wrapper{
    top: 0;
    left:0;
    z-index: 100;
    width: 100%;
    height: 680px;
    display: none;
    opacity: 0.2;
    position: absolute;
    background: #d3d3d3;
  }
  .loader-wrapper img{
    padding:40% ;
  }
</style>
<div class="inventory-txt"><?php print l(t('Add product Inventory Rules'), 'inventory-rule-product'); ?></div>
<div class="inventory-txt"><?php print l(t('Add Global Inventory Rules'), 'inventory-rule/update/default'); ?></div>
<div class="loader-wrapper"><img src="<?php print $base_path . drupal_get_path('module', 'findit_commissions'); ?>/assests/loader.gif" /></div>

<div class="changes changes-header" style="display:none;">
  <button id="discardChanges" class="discardChanges"><?php print t('Discard'); ?></button>
  <button id="presureChanges" class="presureChanges"><?php print t('Save');?></button>
  <p class="changes-message"></p>
</div>
<div>
  <div class ="category-tree inventory-category-tree" id="categoryTree">

  </div>

  <div class="update-inventory-wrapper" style="display:none;">
    <div class="update-inventory-row">
      <label for="txt-inventory-control" id="lbl-inventory"></label>

      <label for="txt-inventory-soft"> Soft Value</label>
      <input name="txt-inventory-soft" type="text" id="inventory-value-soft">
    </div>

    <div class="update-inventory-row">
      <label for="txt-inventory-hard"> Hard Value </label>
      <input name="txt-inventory-hard" type="text" id="inventory-value-hard">

      <label for="txt-inventory-email-group"> Email Group</label>
      <select id="inventory-email-group">
        <?php foreach ($variables['data'] as $key => $group) { ?>
          <option value="<?php print $key; ?>"><?php print $group; ?></option>
        <?php } ?>
      </select>
    </div>
    <div class="error-for-txt-commission"></div>
    <button id="updateCommission" class="btn btn-primary">Update</button>
    <button id="cancelCommission" class="btn btn-primary">Cancel</button>
    <button style="display:none;" id="resetCommission" class="btn btn-primary">Reset</button>
  </div>
</div>
<div class="changes" style="display:none;">
  <button id="discardChanges" class="discardChanges"><?php print t('Discard'); ?></button>
  <button id="presureChanges" class="presureChanges"><?php print t('Save');?></button>
  <p class="changes-message"></p>
</div>

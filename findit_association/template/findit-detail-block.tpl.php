<?php
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
?>

<div class = "main-container row" id = "seller-top-detail">
    <div class="left-site col-md-3 col-xs-12">
        <div class='image-div'>
            <?php print render(field_view_field('commerce_store', $store->value(), 'field_store_logo'));
            ?>
        </div>
        <div class="seller-rating">
            <?php print t("Seller Rating : @value", array('@value' => $rating)); ?>
        </div>
    </div>
    <div class="right-side-block col-md-9 col-xs-12">
        <div class="right-side-right col-md-6 col-xs-12">
            <div class='seller-name'>
                <div class="label-info">
                    <?php print t("Seller Name :"); ?>
                </div>
                <div class="value">
                    <?php print $store->title->value(); ?>
                </div>
            </div>
            <div class='email'>
                <div class="label-info">
                    <?php print t("Email Id:"); ?>
                </div>
                <div class="value">
                    <?php print $user->mail; ?>
                </div>
            </div>
            <div class='account-status'>
                <div class="label-info">
                    <?php print t("Account Status:"); ?>
                </div>
                <div class="value">

                    <?php print $store->status->value() ? t('Activate') : t("Deactivate") ; ?>
                </div>
                <span class="link disable-link btn btn-primary">

                    <?php print $store->status->value() ? l(t('Deactivate'), 'astore/' . $store->store_id->value() . '/deactivate/status', array('query' => drupal_get_destination())) : l(t('Activate'), 'astore/' . $store->store_id->value() . '/activate/status', array('query' => drupal_get_destination())) ; ?>
                </span>
            </div>
        </div>
        <div class="right-side-left col-md-6 col-xs-12">
            <div class='resgiser-date'>
                <div class='label-info'>
                    <?php print t('Registered Since :') ?>
                </div>
                <div class="value">
                    <?php print date('d M Y', $user->created); ?>
                </div>
            </div>
            <?php if(isset($profile->uid)){ ?>
            <div class='seller-name'>
                <div class="label-info">
                    <?php print t("Contact Person :"); ?>
                </div>
                <div class="value">
                    <?php print $profile->field_full_name->value(); ?>
                </div>
            </div>
            <?php } ?>
            <?php if (isset($profile->field_address)) { ?>
              <div class='seller-phone'>
                  <div class="label-info">
                      <?php print t("Conatct Number :"); ?>
                  </div>
                  <div class="value">
                      <?php $address = $profile->field_address->value(); ?>
                      <?php print $address['mobile_number']; ?>
                  </div>
              </div>
            <?php } ?>
        </div>
        <div class="right-side-bottom col-md-12 col-xs-12">
            <div class="view-orders">
                <?php print l(t("View Orders"), '/merchant-store/' . $store->unique_store_id->value()); ?>
            </div>
            <div class="view-inventory">
                <?php print l(t(" View Products"), 'astore/' . $store->store_id->value() . '/associations'); ?>
            </div>
            <div class="view-orders">
                <?php print l(t("Storefront"), 'user/' . $store->uid->value() . '/edit/store'); ?>
            </div>
        </div>
    </div>
</div>
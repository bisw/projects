<?php
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
?>
<div class="clearfix seller-reaview-rate">
  
    <div class="row">
    <h4 class="heading"><?php print t('Your Current Rating'); ?></h4>
        <div class="col-md-2 col-sm-6 average-rating">
            <i class="glyphicon glyphicon-star">
                <span class="rate-count"> <?php print $rating['rating']['avg'] ?></span>
            </i>
            <span> <?php print t('Average Ratings'); ?></span>
        </div>
        <div class="col-md-4 col-sm-6 seller-review-bars">
            <ul class="rating-scale">
                <?php
                foreach ($rating['rating']['count'] as $key => $val) {
                  $star_label = $key / 20;
                  $percent = $val * 100 / $rating['rating']['total'];
                  ?>
                  <li>
                      <div class="rate-bar"><?php print t('@label Star', array('@label' => $star_label)); ?></div>
                      <div class="progress">
                          <div style="width:<?php print t('@per%', array('@per' => $percent)) ?>" aria-valuemax="<?php print $rating['rating']['total_of_rating'] ?>" aria-valuemin="0" aria-valuenow="<?php print $val ?>" role="progressbar" class="progress-bar">
                              <span class="sr-only"><?php print t('@per% Complete', array('@per' => $percent)) ?></span>
                          </div>
                      </div>                                
                      <div class="rate-bar"><?php print $val; ?></div>
                  </li>
                  <?php
                }
                ?>

            </ul>
        </div>
        <div class="col-md-6 col-sm-12 rating-property">
            <ul class="clearfix">
                <li class="<?php print $extra_data['0'] ?>">
                    <i class="seller-property-ico hassel-icon1 icon-Haslefreereturn"></i>
                    <p><?php print t('Hassle Free Return'); ?></p>
                </li>
                <li class="<?php print $extra_data['1'] ?>">
                    <i class="seller-property-ico hassel-icon2 icon-ReplacementWarranty-01"></i>
                    <p><?php print t('30 Days Replacement Guarantee'); ?></p>
                </li>
                <li class="<?php print $extra_data['2'] ?>">
                    <i class="seller-property-ico hassel-icon3 icon-GiftWrap"></i>
                    <p><?php print t('Gift Wrap Facility'); ?></p>
                </li>
            </ul>
        </div>
    </div>
</div>

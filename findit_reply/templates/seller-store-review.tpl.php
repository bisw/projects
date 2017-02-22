<?php
/**
 * @file
 * Template file for display of store review in merchant panel.
 */

$path = drupal_get_path('module', 'fivestar');
drupal_add_css($path . '/css/fivestar.css');
$review = $data['review'];
$ptitle = $data['ptitle'];
$product_title = findit_get_product_title($data['review']['sku']);
$reviewdate_val = $data['reviewdate_val'];
?>
<div class="row costumer-review-section no-gutters"  cid="<?php print $review['id'] ?>" id="review-main-thread- <?php print $review['id']; ?>">
    <div class="col-sm-1 nopadding"><div class="reviewer-dp"><img src="<?php print $review['image'] ?>" alt="<?php print $review['name'] ?>"><?php print $review['user_name'] ?><div class="rateit"> <?php
                print theme('fivestar_static', array(
                  'rating' => $review['rating'],
                  'widget' => array(
                    'name' => 'basic',
                    'css' => $path . '/widgets/basic/basic.css',
                  ),
                )
                    )
                ?></div></div></div>
    <div class="col-sm-9"><div class="comment-desc"><div class="column-group"><a class="product-title" href="<?php print $data['result_title_link']; ?>"><?php print $data['result_title'] ?>"</a>
<!--                <div class="product-title-seller"><?php //print $product_title ?></div>-->
                <div class="product-review-topwrapper"><?php print $ptitle; ?></div>            
                <h3><a href="javascript:void(0)"><?php print $review['subject'] ?></a> &nbsp; <?php print $reviewdate_val ?></h3>
            </div>
            <div class="review-main-body comment more">
                <span class="more"><?php print $review['description'] ?></span>
            </div>
            <div class="product-icon"><?php print $data['image'] ?></div>
            <div class="review-toggle-section related-cmnt-nav">
                <div class="comment-count"><?php print $data['comments'] ?></div>
                <span class="thumb-yes-no pull-left">
                    <a><i class="icon-Yes"></i> <?php print t('yes(@likecount)', array('@likecount' => $review['like'])); ?></a>
                    <a><i class="icon-No"></i> <?php print t('no(@dislikecount)', array('@dislikecount' => $review['dislike'])); ?></a>
                </span>
                <div class="review-report-abuse pull-left"><?php print t('Reported as Abuse (@report_count)', array('@report_count' => $review['report'])); ?></div>
                <?php print $data['report_to_findit']; ?></div>
            <div class="review-reply-body-wrap">
                <div class="review-reply-body-<?php print $review['id'] ?>"></div>
            </div>
        </div>
    </div>
    <?php print $data['review_share']; ?><div><?php print $data['reply_block']; ?></div>
</div>

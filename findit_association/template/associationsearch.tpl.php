<?php echo drupal_render($form['mode']) ?>
<div class="clearfix">
  <i class="icon-ReportIssue"></i>
  <span><?php print t('Select the category and sub-category or search to proceed.'); ?></span>
</div>
<?php echo drupal_render($form['title']) ?>
<?php echo drupal_render($form['category']) ?>
<?php echo drupal_render_children($form); ?>

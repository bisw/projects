<div class="inventory-search-block page-title">
 <!-- <h1 class="pull-left">Inventory</h1> -->
 <div class="inverntorySearch">
    <?php
		    print '<div class="pull-left">' . drupal_render($form['store']) . '</div>';
        print '<div class="pull-right search-wrapper">' . drupal_render($form['term']) .drupal_render($form['submit']). '</div>';
    ?>
</div>
 <?php echo drupal_render_children($form); ?>

/**
 * @file
 * Report to findit accept/reject by findit related jQuery functionalities.
 */
(function ($) {
    Drupal.behaviors.reportToFindit = {
        attach: function (context, settings) {
            $('.findit-ignore-report-to-findit-link').click(function (e) {
                var review_id = $(this).attr("review_id");
                $.ajax({
                    type: 'POST',
                    url: settings.basePath + 'ignore-raise-to-findit/' + review_id,
                    success: function (data) {
                        if (data == 'success') {
                            // Remove table row on ignore.
                            $(".findit-ignore-raise-to-findit-" + review_id).closest('tr').remove();
                        }
                    },
                    error: function (xhr, status, error) {
                    },
                });
            });
        }
    };
})(jQuery);
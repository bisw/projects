/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
(function ($) {
    Drupal.behaviors.pagechange = {
        attach: function (context, settings) {

            var count = 0;
            $('#processcart-table input[id^="edit-rows"],#processcart-table select[id^="edit-rows"]').bind('change keyup paste', function () {
                count++;

            })

            $('.pagination a').click(function (e)
            {

                if (count > 0)
                {
                    e.preventDefault();
                    var msg = Drupal.t('Save all changes on your page before proceeding to next page');
                    if ($("#save-msg").length == 0) {
                        $('ul.pagination').after('<div id="save-msg"><p>' + msg + '</p></div>');
                    }
                }
            });

            $('.back-to-inventory').click(function ()
            {
                count = 0;
            })
        }
    }
})(jQuery);
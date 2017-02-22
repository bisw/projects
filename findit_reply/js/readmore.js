/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
//(function ($) {
//    alert("here");
//})(jQuery);
(function ($) {

    // here $ would be point to jQuery object
    $(document).ready(function () {
        // Configure/customize these variables.
        var showChar = 375;  // How many characters are shown by default
        var ellipsestext = "...";
        var moretext = Drupal.t("view more ");
        var lesstext = Drupal.t("view less");


        $('.more').each(function () {
            var content = $(this).html();

            if (content.length > showChar) {

                var c = content.substr(0, showChar);
                var h = content.substr(showChar, content.length - showChar);

                var html = c + '<span class="moreellipses">' + ellipsestext + '&nbsp;</span><span class="morecontent"><span>' + h + '</span></span>&nbsp;&nbsp;<a href="" class="morelink">' + moretext + '</a></span>';

                $(this).html(html);
            }

        });


        $(".morelink").click(function () {
            $(this).parent().find('.moreellipses').toggle();
            if ($(this).hasClass("less")) {
                $(this).removeClass("less");
                $(this).html(moretext);
            } else {
                $(this).addClass("less");
                $(this).html(lesstext);
            }
           // $(this).parent().prev().toggle();
            $(this).prev().find('.morecontent').toggle();
            return false;
        });

    });
})(jQuery);

(function($) {
    Drupal.behaviors.associationModule = {
        attach: function(context, settings) {
            var butTitle = 'Association Bucket';
            $('.addtocart', context).click(function() {
                var ele = $(this);
                var product_id = $(this).attr('product');
                var opr = $(this).attr('opr');
                //Add loading img
                $('.loading').show();
                $.ajax({
                    url: base_path + "merchant/association/service/?opr=" + opr + "&product=" + product_id,
                }).done(function(data) {
                    if (data.result == true) {
                        if (opr == 'addtocart') {
                            ele.attr('opr', 'removetocart');
                            ele.attr('value', 'Remove to Bucket');
                            console.log(processcart_totalitem);
                            processcart_totalitem++;
                            $('#processcart_but').html(butTitle + ' (' + processcart_totalitem + ')');
                        } else {
                            ele.attr('opr', 'addtocart');
                            ele.attr('value', 'Add to Bucket');
                            processcart_totalitem--;
                            $('#processcart_but').html(butTitle + ' (' + processcart_totalitem + ')');
                        }
                    }
                    $('.loading').hide();
                });
            });

            $('#processcart_but').click(function() {
                if (processcart_totalitem > 0) {
                    return true;
                } else {
                    alert('Empty Bucket !!!');
                    return false;
                }
            });

            setInterval(function() {
                $(".chosen-processed").hide();
            }, 1);
        }
    };

    function submitUnArchieve(e) {
        if ($('#findit-merchant-inventory-list-form input[type="checkbox"]:checked').length) {

            e.preventDefault();
            $('#findit-merchant-inventory-list-form').unbind('submit').bind('submit', function(e) {

                e.preventDefault();
            });
            var cp = new Drupal.Confirm({header: 'Do you want to unarchive selected SKU(s)?', message: ''});
            cp.on('yes', function() {
                $('#findit-merchant-inventory-list-form').unbind('submit');
                $('button[value="Un-Archive"]').unbind('click', submitUnArchieve);
                $('button[value="Un-Archive"]').trigger('click');
            }.bind(this));
            cp.on('no', function() {
                $('#findit-merchant-inventory-list-form').unbind('submit');
            }.bind(this));
            cp.on('close', function() {
                $('#findit-merchant-inventory-list-form').unbind('submit');
            }.bind(this));
        }
    }
    Drupal.behaviors.finditMerchantStoreList = {
        attach: function(context, settings) {
            $('button[value="Un-Archive"]').bind('click', submitUnArchieve);
            $('.deactive_store').on('click', function() {
                var store_id = $(this).attr('store_id');
                $("[id^=edit-table-]").attr("checked", false);
                $('#edit-table-' + store_id).attr("checked", "checked");
                $('#edit-operation').val("disable");
                $('#findit-admin-store-list-form').submit();
            });

            $('.active_store').on('click', function() {
                var store_id = $(this).attr('store_id');
                $("[id^=edit-table-]").attr("checked", false);
                $('#edit-table-' + store_id).attr("checked", "checked");
                $('#edit-operation').val("enable");
                $('#findit-admin-store-list-form').submit();
            });
        }
    }

    Drupal.behaviors.finditMerchantAssociationList = {
        attach: function(context, settings) {
            $('.deactive_sku').on('click', function() {
                var association_id = $(this).attr('association_id');
                $('#edit-table-' + association_id).attr("checked", "checked");
                $('#edit-operation').val("disable");
                $('#findit-admin-store-association-list-form').submit();
            });

            $('.active_sku').on('click', function() {
                var association_id = $(this).attr('association_id');
                $('#edit-table-' + association_id).attr("checked", "checked");
                $('#edit-operation').val("enable");
                $('#findit-admin-store-association-list-form').submit();
            });
        }
    }

    Drupal.behaviors.processcart = {
        attach: function(context, settings) {
            $('input[name*="][status]"]').on('click', function() {
                var default_value = $(this).attr('default_value');
                if (default_value == 1 && !$(this).attr('checked')) {
                    if (confirm("Are you sure you want to disable SKU ?")) {
                        return true;
                    }
                    return false
                }
            });

            $(window).scroll(function(event) {
                if ($('.sticky-header').css('visibility') == 'visible') {
                    $('#findit-merchant-association-search #edit-actions,#findit-merchant-association-processcart #edit-actions').addClass('sticky-cbuttons');
                } else {
                    $('#findit-merchant-association-search #edit-actions,#findit-merchant-association-processcart #edit-actions').removeClass('sticky-cbuttons');
                }
            });
        }
    }

    /**************** New js for theme *******************/
    Drupal.behaviors.theme = {
        attach: function(context, settings) {
            $('.btn-edit,.btn-archive,.btn-save,.btn-cancel,.btn-download').on('click', function() {
                $('#action').val($(this).val());
                $('#findit-merchant-inventory-list-form').submit();
            });

            // Auto submit association global search on dropdown selection.
            $('.pull-right.search-wrapper .form-type-textfield').on('click', '.dropdown li', function() {
                $('#findit-association-globalsearch-form').submit();
            });

            // Reload page on close pop up button.
            if ($('body').hasClass('page-merchant-inventory')) {
              $('button.close').bind('click', function() {
                location.reload();
              });
            }

            // Disable association blocked by admin.
            if ($('body').hasClass('page-merchant-inventory')) {
              if ($('body').find('.blocked-association').length > 0) {
                $(".blocked-association").each(function(index) {
                  $('tr td #edit-rows-' + $(this).attr('id')).attr('disabled', 'disabled');
                  $('tr td .form-item-rows-' + $(this).attr('id')).html('');
                });
              }
            }

            // Scroll to top on edit association.
            if (window.location.href.indexOf('all?opr=edit') != -1) {
              if($(document).find('.scrollTopOffset').length > 0){
                var topScroll = $(document).find('.scrollTopOffset').offset().top - 220;
                $(window).scrollTop(topScroll);
              }
            }

            // Enable next page when select assoc..
            if (!$('#findit-merchant-association-list-new .nextstep.pull-right').hasClass('active')) {
                $('#findit-merchant-association-list-new').on('click', 'input[type="checkbox"]', function() {
                    if ($('#findit-merchant-association-list-new input[type="checkbox"]:checked').length > 0) {
                        $('#findit-merchant-association-list-new .nextstep.pull-right').removeClass('inactive');
                        $('#findit-merchant-association-list-new .nextstep.pull-right').addClass('active');
                        $('#findit-merchant-association-list-new .addtolist.pull-right').removeClass('disabled');
                        $('#findit-merchant-association-list-new .addtolist.pull-right').addClass('enabled');
                    } else {
                        $('#findit-merchant-association-list-new .nextstep.pull-right').removeClass('active');
                        $('#findit-merchant-association-list-new .nextstep.pull-right').addClass('inactive');
                         $('#findit-merchant-association-list-new .addtolist.pull-right').removeClass('enabled');
                        $('#findit-merchant-association-list-new .addtolist.pull-right').addClass('disabled');
                    }
                });
            }

            // Update Association cart on select/deselect all.
            $('#findit-merchant-association-list-new').on('click', '.select-all input[type="checkbox"]', function() {
                if ($(this).attr('checked')) {

                    // Add asso to cart.
                    var AddElements = $('#findit-merchant-association-list-new table tbody').find('[name^="table"]:checked');
                    if (AddElements.length > 0) {
                        var addProducts = new Array();
                        AddElements.each(function(index) {
                            addProducts.push($(this).val());
                        });
                        AddNewAssociationToCart(addProducts);
                    }
                } else {

                    // Remove asso from cart.
                    var removeElements = $('#findit-merchant-association-list-new table tbody').find('[name^="table"]:not(:checked)');
                    if (removeElements.length > 0) {
                        var removeProducts = new Array();
                        removeElements.each(function(index) {
                            removeProducts.push($(this).val());
                        });
                        RemoveAssoFromCart(removeProducts);
                    }
                }
            });

            // Update Association cart on select/deselect of single row.
            $('#findit-merchant-association-list-new input[type="checkbox"]').on('click', function() {
                // Add association in asso cart when merchant checked the association row.
                if ($(this).attr('checked')) {

                    // Add asso to cart.
                    var addProducts = new Array();
                    addProducts.push($(this).val());
                    AddNewAssociationToCart(addProducts);
                } else {

                    // Remove asso from cart.
                    var removeProducts = new Array();
                    removeProducts.push($(this).val());
                    RemoveAssoFromCart(removeProducts);
                }
            });

            $('#btn-addtolist').on('click', function() {
                var elements = $('#findit-merchant-association-list-new').find('[name^="table"]:checked');
                if (elements.length > 0) {
                    var products = new Array();
                    elements.each(function(index) {
                        products.push($(this).val());
                    });

                    $.ajax({
                        url: base_path + "merchant/association/service/?opr=addtocart&product=" + products.toString(),
                    }).done(function(data) {
                        var message = data.message;
                        if (data.result) {
                            $('.flash-success').html('<div class="alert alert-info alert-dismissable"><button aria-hidden="true" data-dismiss="alert" class="close" type="button">×</button><div class="text-center">' + message + '</div></div>');
                            $('#viewlistbtn').on('click', function() {
                                $('#viewlistmain').trigger('click');
                            });
                        }
                    });
                } else {
                    $('.flash-error').html('<div class="alert alert-danger alert-dismissable"><button aria-hidden="true" data-dismiss="alert" class="close" type="button">×</button><div class="text-center">No Association selected.</div></div>');
                }
            });

            // Next step flow.
            $('#btn-nextstep').on('click', function() {
                //var elements = $('#findit-merchant-association-list-new').find('[name^="table"]:checked');
                var redirect_url = base_path + 'merchant/' + store_id + '/inventory/processcart';
                window.location.href = redirect_url;

//                if (elements.length > 0) {
//                    var products = new Array();
//                    elements.each(function(index){
//                        products.push($(this).val());
//                    });
//
//                    $.ajax({
//                        url: base_path + "merchant/association/service/?opr=addtocart&product=" + products.toString(),
//                    }).done(function(data) {
//                        window.location.href = redirect_url;
//                    });
//                }else{
//                    window.location.href = redirect_url;
//                }
            });

            $('#viewlistbtn').on('click', function() {
                $('#viewlistmain').trigger('click');
            });

            $('.removefromcart-viewlist').on('click', function() {
                var href = $(this).attr("href");
                var product_id = $(this).attr('product');
                var row_id = '#row-' + product_id;

                $.ajax({
                    url: href,
                }).done(function(data) {
                    if (data.result) {
                        $(row_id).fadeOut(400, function() {
                            $(row_id).remove();
                            var rowCount = $('#viewlist-table>tbody>tr').length;
                            if (rowCount < 1) {
                                $('#modalContent .close').trigger('click');
                                location.reload();
                            }
                        });
                    }
                });
                return false;
            });

            $('.removefromcart-processcart').on('click', function() {
                var href = $(this).attr("href");
                var product_id = $(this).attr('product');
                var row_id = '#row-' + product_id;

                $.ajax({
                    url: href,
                }).done(function(data) {
                    if (data.result) {
                        $(row_id).fadeOut(400, function() {
                            $(row_id).remove();
                            var rowCount = $('#processcart-table>tbody>tr').length;
                            if (rowCount < 1) {
                              location.reload();
                            }
                        });
                    }
                });
                return false;
            });

            $('#globalsearchbtn').on('click', function() {
                $('#findit-association-globalsearch-form').submit();
            });

            var queryString = GetParameterValues('opr');
            var header = '';
            if (queryString == 'edit') {
                var header = '<tr><th></th><th></th><th colspan="2">Stock</th><th  colspan="2">Price(RM)</th><th></th></tr>';
            } else {
                var header = '<tr><th></th><th></th><th></th><th colspan="2">Stock</th><th  colspan="2">Price(RM)</th><th></th></tr>';
            }
            $('#findit-merchant-inventory-list-form table thead').prepend(header);

            var header = '<tr><th></th><th></th><th colspan="2">Stock</th><th  colspan="2">Price(RM)</th><th></th><th></th></tr>';
            if ($('#findit-merchant-inventory-processcart table thead tr').length == 1) {
                $('#findit-merchant-inventory-processcart table thead').prepend(header);
            }


            function GetParameterValues(param) {
                var url = window.location.href.slice(window.location.href.indexOf('?') + 1).split('&');
                for (var i = 0; i < url.length; i++) {
                    var urlparam = url[i].split('=');
                    if (urlparam[0] == param) {
                        return urlparam[1];
                    }
                }
            }

            setInterval(function() {
                $('#modalContent,#modal-content').height('auto');
            }, 1);

            $('#findit-association-bulk-upload').submit(function() {
                var filefield = $('#findit-association-bulk-upload #edit-file');
                if (filefield.get(0).files.length > 0) {
                    var ext = filefield.val().split('.').pop().toLowerCase();
                    if ($.inArray(ext, ['csv']) == -1) {
                        filefield.val(null);
                        showMSG('invalid extension!!!', 'error');
                        return false;
                    }
                } else {
                    $('#findit-association-bulk-upload #edit-file').trigger('click');
                    return false;
                }
            });

            $('#findit-association-bulk-upload #edit-file').on('change', function() {
                $('#findit-association-bulk-upload').submit();
            });

            function showMSG(message, type) {
                var msg_class = '';
                switch (type) {
                    case 'error' :
                        msg_class = 'alert-danger';
                        break;
                    case 'success' :
                        msg_class = 'alert-success';
                        break;
                    case 'info' :
                        break;
                }
                $('#page #header').after('<div id="messages"><div class="section clearfix"><div class="alert alert-block ' + msg_class + '"><a href="#" data-dismiss="alert" class="close">×</a><h4 class="element-invisible">Status message</h4>' + message + '</div></div></div>');
            }

            // Callback function to ad association in the asso cart.
            function AddNewAssociationToCart(addProducts) {
                if (addProducts.length > 0) {
                    $.ajax({
                        url: base_path + "merchant/association/service/?opr=addtocart&product=" + addProducts.toString(),
                    }).done(function(data) {
                        return true;
                    });
                }
                return false;
            }

            // Callback function to remove association from asso cart.
            function RemoveAssoFromCart(removeProducts) {
                if (removeProducts.length > 0) {
                    $.ajax({
                        url: base_path + 'merchant/association/service?opr=removetocart&product=' + removeProducts.join(','),
                    }).done(function(data) {
                        return true;
                    });
                }
                return false;
            }

        }
    }

}(jQuery));

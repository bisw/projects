/*
 * Srijan Technology Pvt. Ltd. India
 * (c) Ajay Singh<meajaysingh@hotmail.com>
 * createdOn: 26/07/2015
 * updatedOn:29/07/2015
 *
 *
 */
/*global Drupal, jQuery*/
(function($) {
    'use strict';
    var InventoryControlTree = (function() {


        function InventoryControlTree(pConfig) {
            this.config = pConfig;
            this.treeOption = pConfig.treeOption || {};
            this.updatedCommissions = [];
            this.currentNode = undefined;
            this.rest_url = pConfig.rest_url;
            this.actualTree = this.config.selector + ' #actual-tree';
        }

        InventoryControlTree.prototype.refreshChangeArea = function() {
            if (this.updatedCommissions.length) {
                $(this.config.changes).show('slow');
            } else {
                $(this.config.changes).hide('slow');
            }
        };
        InventoryControlTree.prototype.getData = function(cb) {
            $.getJSON(Drupal.settings.basePath+this.rest_url, cb.bind(this));
        };
        InventoryControlTree.prototype.postData = function(cb) {
            $.post(Drupal.settings.basePath+this.rest_url, {
                data: JSON.stringify(this.updatedCommissions)
            }, cb.bind(this));
        };
        InventoryControlTree.prototype.init = function(data) {
            if (data) {
                this.treeOption.data = data;
                $(this.config.selector).empty().append('<div id="actual-tree"></div>');
                $(this.actualTree).tree(this.treeOption);
                this.bindEvents();
            } else {
                loaderCommission(true);
                this.getData(function(_reponse) {
                    this.treeOption.data = _reponse;
                    $(this.config.selector).empty().append('<div id="actual-tree"></div>');
                    $(this.actualTree).tree(this.treeOption);
                    this.bindEvents();
                    loaderCommission();
                }, loaderCommission);
            }
            return this;
        };
        InventoryControlTree.prototype.bindEvents = function() {
            $(this.actualTree).unbind('tree.click').bind('tree.click', this.treeClickedEvent.bind(this));
            $(this.config.resetBtn).unbind('click').bind('click', this.resetBtnEvent.bind(this));
            $(this.config.updateBtn).unbind('click').bind('click', this.treeUpdateBtnEvent.bind(this));
            $(this.config.cancelBtn).unbind('click').bind('click', this.cancelBtnEvent.bind(this));
            $(this.config.saveChanges).unbind('click').bind('click', this.saveChangesOnserver.bind(this));
            $(this.config.discardChanges).unbind('click').bind('click', this.discardCurremtChanges.bind(this));
            $(this.config.inputBox).unbind('change').change(isInputValid.bind(this));
            return this;
        };
        InventoryControlTree.prototype.treeClickedEvent = function(e) {
            e = e || event;
            if (e.node) {
//                if (Drupal.settings.commission_page_view !== 1) {
                    this.updatedCommissionBox(e.node);
                    $(this.config.resetBtn).hide();
                    if (e.node.dirty == 1) {
                        $(this.config.resetBtn).show();
                    }
//                }
            }
        };
        InventoryControlTree.prototype.updatedCommissionBox = function(_node) {
            this.currentNode = parseNode(_node);
            var updateC = this.updatedCommissions.filter(function(_uc) {
                return _uc.tid === _node.tid;
            })[0];
            if (updateC) {
                this.currentNode = updateC;
            }
            $(this.config.inputLbl).html(Drupal.t('Enter Inventory values for ') + this.currentNode.name);
            $('#inventory-value-soft').val(this.currentNode.soft_limit);
            $('#inventory-value-hard').val(this.currentNode.hard_limit);
            $('#inventory-email-group').val(this.currentNode.email_group);
            $(this.config.commissionUpdateArea).show();
        };
        InventoryControlTree.prototype.treeUpdateBtnEvent = function() {
            
            var soft_value, hard_value, email_group;
            try {
                //TODO read all new values here
              soft_value   = parseInt($('#inventory-value-soft').val(), 10);
              hard_value   = parseInt($('#inventory-value-hard').val(), 10);
              email_group =  $('#inventory-email-group').val(); 
               $("html, body").animate({scrollTop:0},'2000');
            } catch (e) {
                $('.error-for-txt-inventory').html('Error while processing. please check the input values.');
                return;
            }
//            if (!newValue || newValue > 99) {
//                $(this.config.inputBoxError).html('Please enter a valid number!');
//                return this;
//            }
            $(this.config.commissionUpdateArea).hide();
            if (this.currentNode.soft_limit === soft_value && this.currentNode.hard_limit === hard_value && this.currentNode.email_group === email_group) {
                return this;
            }
            //all pass set the new values to the current node
            this.currentNode.soft_limit = soft_value;
            this.currentNode.hard_limit = hard_value;
            this.currentNode.email_group = email_group;

            var __tid = this.currentNode.tid;
            var foundOld = this.updatedCommissions.filter(function(_item) {
                return _item.tid === __tid;
            })[0];
            if (foundOld) {
                foundOld.soft_limit = soft_value;
                foundOld.hard_limit = hard_value;
                foundOld.email_group = email_group;
            } else {
                this.updatedCommissions.push(copyObject(this.currentNode));
            }
            this.updateMessage('There are unsaved Changes.');
            this.refreshChangeArea();
            return this;
        };
        InventoryControlTree.prototype.resetBtnEvent = function() {
            var __tid = this.currentNode.tid;
            var foundOld = this.updatedCommissions.filter(function(_item) {
                return _item.tid === __tid;
            })[0];
            if (foundOld) {
                foundOld.reset = 1;
            } else {
                this.currentNode.reset = 1;
                this.updatedCommissions.push(copyObject(this.currentNode));
            }
            $(this.config.commissionUpdateArea).hide();
            this.updateMessage('There are unsaved Changes.');
            this.refreshChangeArea();
            return this;
        };
        InventoryControlTree.prototype.cancelBtnEvent = function() {
            var __tid = this.currentNode.tid;
            this.updatedCommissions = this.updatedCommissions.filter(function(_cItem) {
                return _cItem.tid !== __tid;
            });
            $(this.config.commissionUpdateArea).hide();
            this.refreshChangeArea();
            return this;
        };
        InventoryControlTree.prototype.discardCurremtChanges = function() {
            this.updatedCommissions = [];
            this.refreshChangeArea();
            this.updateMessage('All Changes has been deleted.', true);
            return this;
        };
        InventoryControlTree.prototype.saveChangesOnserver = function() {
            var self = this;
            loaderCommission(true);
            this.postData(function(_reponse) {
                self.updateMessage('Changes Saved!', true);
                this.updatedCommissions = [];
                this.refreshChangeArea();
                this.init(_reponse);
                loaderCommission();
            }, loaderCommission);
            return this;
        };
        InventoryControlTree.prototype.updateMessage = function(msg, isTemp) {
            var status = $(this.config.status);
            if (isTemp) {
                setTimeout(function() {
                    status.html('');
                }, 2500);
            }
            status.html(msg);
            return this;
        };

        function loaderCommission(display) {
            if (display) {
                $('.loader-wrapper').show();
            } else {
                $('.loader-wrapper').hide();
            }
        }

        function copyObject(obj) {
            try {
                return JSON.parse(JSON.stringify(obj));
            } catch (e) {
                return {};
            }
        }

        function parseNode(_node) {
            var obj = {};
            obj.tid = _node.tid;
            obj.name = _node.name;
            obj.soft_limit = _node.soft_limit;
            obj.hard_limit = _node.hard_limit;
            obj.email_group = _node.email_group;
            obj.dirty = _node.dirty;
            return obj;
        }

        function isInputValid(e) {
            e = (e || event);
            var newValue;
            try {
                newValue = parseInt($(this.config.inputBox).val(), 0);
            } catch (_E) {

            }
            if (!newValue) {
                $(this.config.inputBoxError).html('Please enter a valid number!');
                return this;
            } else {
                $(this.config.inputBoxError).html('');
                return this;
            }
        }

        return InventoryControlTree;
    })();

    var commissiontree = new InventoryControlTree({
        selector: '#categoryTree',
        commissionUpdateArea: '.update-inventory-wrapper',
        inputLbl: '#lbl-inventory',
        cancelBtn: '#cancelCommission',
        updateBtn: '#updateCommission',
        resetBtn: '#resetCommission',
        saveChanges: '.presureChanges',
        discardChanges: '.discardChanges',
        status: '.changes-message',
        changes: '.changes',
        rest_url:'findit-inventory-rules',
        treeOption: {
            autoOpen: 0,
            label: 'name'
        }
    }).init();
    Drupal.behaviors.findit_inventory_control_tree = {
        attach: function(context, settings) {
            commissiontree.bindEvents();
        }
    };
})(jQuery);

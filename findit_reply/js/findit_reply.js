/*
 * Srijan Technology Pvt. Ltd. India
 * (c) Ajay Singh<meajaysingh@hotmail.com>
 * createdOn: 10/08/2015
 * updatedOn:14/08/2015
 *
 *
 */
/*global jQuery, Drupal,Backbone, Handlebars, _, HandlebarsIntl*/
(function($, Drupal, Backbone, Handlebars, HandlebarsIntl) {
    'use strict';
    $.fn.serializeObject = function() {
        var o = {};
        var a = this.serializeArray();
        $.each(a, function() {
            if (o[this.name] !== undefined) {
                if (!o[this.name].push) {
                    o[this.name] = [o[this.name]];
                }
                o[this.name].push(this.value || '');
            } else {
                o[this.name] = this.value || '';
            }
        });
        return o;
    };
    HandlebarsIntl.registerWith(Handlebars);


    function parseData(_objArray) {
        var ret_obj = [];
        if (!Array.isArray(_objArray)) {
            _objArray = [_objArray];
        }

        function parseObj(_a) {
            _a.created = new Date(parseInt(_a.created + '000'));
            _a.status = parseInt(_a.status, 0);
            _a.canPublish = Drupal.settings.reply_store_id ? false : true;
            _a.children_total = parseInt(_a.children_total, 0);
            return _a;
        }
        for (var i = 0; i < _objArray.length; i++) {
            ret_obj.push(parseObj(_objArray[i]));
        }
        return ret_obj;
    }

    function getTemplate(id) {
        return Handlebars.compile($('#' + id).html());
    }


    function ApiHandler(pOps) {
        this.options = $.extend({}, pOps);
    }
    ApiHandler.prototype.get = function() {
        if (Drupal.settings.reply_store_id) {
            this.options.query = (this.options.query || {});
            this.options.query.store_id = Drupal.settings.reply_store_id;
        }
        return this.restify('GET', this.options.query);
    };
    ApiHandler.prototype.post = function(data) {
        return this.restify('POST', data);
    };
    ApiHandler.prototype.put = function(data) {
        return this.restify('PUT', data);
    };
    ApiHandler.prototype.restify = function(method, data) {
        var obj = {};
        obj.url = (method === 'POST') ? this.options.url : (this.options.id ? this.options.url + '/' + this.options.id : this.options.url);
        obj.method = method ? method : 'GET';
        obj.data = data ? data : null;
        obj.contentType = 'application/x-www-form-urlencoded';
        obj['Content-Type'] = 'application/x-www-form-urlencoded';
        obj.dataType = 'json';
        if (method === 'GET') {
            return $.ajax(obj);
        }
        return $.get(Drupal.settings.reply_session).then(function(_token) {
            obj.beforeSend = function(xhr) {
                xhr.setRequestHeader('X-CSRF-TOKEN', _token);
            };
            return $.ajax(obj);
        }.bind(this));

    };


    function ModelCollection(pOptions) {
        this.api = new ApiHandler(pOptions);
        this.array = [];
        this.initialize();

    }
    ModelCollection.prototype.initialize = function() {

    };
    ModelCollection.prototype.fetch = function() {
        this.api.get().then(function(_response) {
            this.pages = _response.pages || {};
            this.items = _response.items || {};
            var data = parseData(_response.data);
            for (var i = 0; i < data.length; i++) {
                this.addData(data[i]);
            }
            this.trigger('fetched');
        }.bind(this));
    };
    ModelCollection.prototype.addData = function(_model) {
        this.array.push(_model);
        this.trigger('add', _model);
    };
    ModelCollection.prototype.on = function(eventName, cb, contaxt) {
        $(this).on(eventName, cb.bind(contaxt));
    };
    ModelCollection.prototype.trigger = function(eventName, data) {
        $(this).trigger(eventName, data);
    };
    ModelCollection.prototype.addNew = function(_model) {
        this.api.post(_model).then(function(_response) {
            this.collection.addData(parseData(_response)[0]);
        });
    };

    // Single Reply Thread
    function ModelSubThreadView(pOptions) {
        this.model = pOptions.model;
        this.api = new ApiHandler({
            url: Drupal.settings.reply_resturl,
            id: this.model.id
        });
        this.tagName = 'li';
        this.cid = _.uniqueId();
        this.template = getTemplate('template-review-reply-thread');
        this.initialize();
    }
    ModelSubThreadView.prototype.initialize = function() {
        this._ensureElement();
    };
    ModelSubThreadView.prototype.render = function() {
        this.$el = $(this.el);
        this.$el.empty();
        this.$el.html(this.template(this.model));
        this.bindEvents();
        return this;
    };
    ModelSubThreadView.prototype._ensureElement = function() {
        var attrs = {};
        if (!this.el) {
            attrs.cid = this.cid;
            attrs.id = 'review-main-thread-' + this.model.id;
            var $el = $('<' + this.tagName + '>').attr(attrs);
            this.el = $el[0];
        }
    };
    ModelSubThreadView.prototype.bindEvents = function() {
        if (!Drupal.settings.reply_store_id) {
            this.$el.find('.reply-help-yes').unbind('click').bind('click', this.replyWasHelpFull.bind(this));
            this.$el.find('.reply-help-no').unbind('click').bind('click', this.replyWasNotHelpFull.bind(this));
            this.$el.find('.reply-report-abuse').unbind('click').bind('click', this.reportAbuse.bind(this));
            this.$el.find('.review-publish-unpublish').unbind('click').bind('click', this.togglePublishStatus.bind(this));
        }
    };
    ModelSubThreadView.prototype.togglePublishStatus = function() {
        this.api.put({
            action: 'publish',
            value: this.model.status ? 0 : 1
        }).then(function(_response) {
            this.model = parseData(_response)[0];
            this.render();
        }.bind(this));
    };
    ModelSubThreadView.prototype.replyWasHelpFull = function() {
        if (!this.model.is_like) {
            this.api.put({
                action: 'like',
                value: 1
            }).then(function(_response) {
                this.model = parseData(_response)[0];
                this.render();
            }.bind(this));
        }

    };
    ModelSubThreadView.prototype.replyWasNotHelpFull = function() {
        if (!this.model.is_dislike) {
            this.api.put({
                action: 'dislike',
                value: 1
            }).then(function(_response) {
                this.model = parseData(_response)[0];
                this.render();
            }.bind(this));
        }
    };
    ModelSubThreadView.prototype.reportAbuse = function() {
        if (!this.model.is_report) {
            this.api.put({
                action: 'report',
                value: 1
            }).then(function(_response) {
                this.model = parseData(_response)[0];
                this.render();
            }.bind(this));
        }
    };
    ModelSubThreadView.prototype.on = function(eventName, cb, contaxt) {
        $(this).on(eventName, cb.bind(contaxt));
    };
    ModelSubThreadView.prototype.trigger = function(eventName, data) {
        $(this).trigger(eventName, data);
    };


    function ModelCollectionSubView(pOptions) {
        this.el = pOptions.el;
        this.parentId = pOptions.id;
        this.collection = new ModelCollection({
            url: Drupal.settings.reply_resturl,
            id: this.parentId
        });
        this.initialize();
    }
    ModelCollectionSubView.prototype.initialize = function() {
        this.$el = $(this.el);
        this.$el.empty();
        this.collection.on('add', this.addOne, this);
    };
    ModelCollectionSubView.prototype.render = function(cb, context) {
        if (cb) {
            this.collection.fetch({
                success: cb.bind(context)
            });
        } else {
            this.collection.fetch();
        }
        return this;
    };
    ModelCollectionSubView.prototype.addOne = function(event, model) {
        this.$el = $(this.el);
        this.$el.append((new ModelSubThreadView({
            model: model
        })).render().el);
        return this;
    };
    ModelCollectionSubView.prototype.bindEvents = function() {

    };






    // Single Main Thread view
    function ModelThreadView(pOptions) {
        this.model = pOptions.model;
        this.api = new ApiHandler({
            url: Drupal.settings.reply_resturl,
            id: this.model.id
        });
        this.tagName = 'li';
        this.cid = _.uniqueId();
        this.template = getTemplate('template-review-main-thread');
        this.isCommentSectionVisible = false;
        this.initialize();
    }
    ModelThreadView.prototype.initialize = function() {
        this._ensureElement();
    };
    ModelThreadView.prototype.render = function() {
        this.$el = $(this.el);
        this.$el.empty();
        this.$el.html(this.template(this.model));
        this.bindEvents();
        return this;
    };
    ModelThreadView.prototype._ensureElement = function() {
        var attrs = {};
        if (!this.el) {
            attrs.cid = this.cid;
            attrs.id = 'review-main-thread-' + this.model.id;
            var $el = $('<' + this.tagName + '>').attr(attrs);
            this.el = $el[0];
        }
    };
    ModelThreadView.prototype.bindEvents = function() {
        this.$el.find('.review-comments').unbind('click').bind('click', this.toggleCommentSection.bind(this));
        this.$el.find('form#frm-review-reply-input').unbind('submit').bind('submit', this.addNewRelyOnIt.bind(this));
        if (!Drupal.settings.reply_store_id) {
            this.$el.find('.review-help-yes').unbind('click').bind('click', this.reviewWasHelpFull.bind(this));
            this.$el.find('.review-help-no').unbind('click').bind('click', this.reviewWasNotHelpFull.bind(this));
            this.$el.find('.review-report-abuse').unbind('click').bind('click', this.reportAbuse.bind(this));
            this.$el.find('.review-publish-unpublish').unbind('click').bind('click', this.togglePublishStatus.bind(this));
        }
    };
    ModelThreadView.prototype.togglePublishStatus = function() {
        this.api.put({
            action: 'publish',
            value: this.model.status ? 0 : 1
        }).then(function(_response) {
            this.model = parseData(_response)[0];
            this.render();
        }.bind(this));
    };
    ModelThreadView.prototype.toggleCommentSection = function() {
        if (this.isBusy) {
            return;
        }
        this.isBusy = true;
        if (this.isCommentSectionVisible) {
            this.$el.find('.review-reply-body-wrap').hide();
            this.isCommentSectionVisible = false;
            this.isBusy = false;
        } else if (!this.model.children_total) {
            this.$el.find('.review-reply-body-wrap').show();
            this.isCommentSectionVisible = true;
            this.isBusy = false;
        } else {
            if (this.replyView) {
                this.$el.find('.review-reply-body-wrap').show();
                this.isCommentSectionVisible = true;
                this.isBusy = false;
            } else {
                this.replyView = new ModelCollectionSubView({
                    el: '.review-reply-body-' + this.model.id,
                    id: this.model.id
                });
                this.replyView.render();
                this.replyView.collection.on('fetched', function() {
                    this.$el.find('.review-reply-body-wrap').show();
                    this.isCommentSectionVisible = true;
                    this.isBusy = false;
                }, this);
            }

        }
    };
    ModelThreadView.prototype.addNewRelyOnIt = function(e) {
        e.preventDefault();
        if (this.isBusy) {
            return;
        }
        this.isBusy = true;
        var obj = $(e.currentTarget).serializeObject();
        if (!obj.description.trim().length) {
            return;
        }
        obj.parent = this.model.id;
        this.api.post(obj).then(function(_response) {
            var obj = parseData(_response)[0];
            this.replyView.collection.addData(obj);
            $(e.currentTarget)[0].reset();
            this.isBusy = false;
        }.bind(this));
    };
    ModelThreadView.prototype.reviewWasHelpFull = function() {
        // if (!this.model.is_like) {
        this.api.put({
            action: 'like',
            value: 1
        }).then(function(_response) {
            this.model = parseData(_response)[0];
            this.render();
        }.bind(this));
        // }

    };
    ModelThreadView.prototype.reviewWasNotHelpFull = function() {
        // if (!this.model.is_dislike) {
        this.api.put({
            action: 'dislike',
            value: 1
        }).then(function(_response) {
            this.model = parseData(_response)[0];
            this.render();
        }.bind(this));
        // }
    };
    ModelThreadView.prototype.reportAbuse = function() {
        if (!this.model.is_report) {
            this.api.put({
                action: 'report',
                value: 1
            }).then(function(_response) {
                this.model = parseData(_response)[0];
                this.render();
            }.bind(this));
        }
    };
    ModelThreadView.prototype.on = function(eventName, cb, contaxt) {
        $(this).on(eventName, cb.bind(contaxt));
    };
    ModelThreadView.prototype.trigger = function(eventName, data) {
        $(this).trigger(eventName, data);
    };












    /** View Collections  */
    function ModelCollectionView(pOptions) {
        this.el = pOptions.el;
        this.collection = new ModelCollection({
            url: Drupal.settings.reply_resturl,
            query: pOptions.query
        });
        this.initialize();
    }
    ModelCollectionView.prototype.initialize = function() {
        this.$el = $(this.el);
        this.collection.on('add', this.addOne, this);
    };
    ModelCollectionView.prototype.render = function() {
        this.collection.fetch();
    };
    ModelCollectionView.prototype.addOne = function(event, model) {
        this.$el = $(this.el);
        this.$el.append((new ModelThreadView({
            model: model
        })).render().el);
        return this;
    };
    ModelCollectionView.prototype.bindEvents = function() {

    };
    /** E view-collection */




    function PagerReview(pOptions) {
        this.pages = pOptions.pages;
        this.el = pOptions.el;
        this.template = getTemplate('template-review-pager');
        this.initialize();
    }
    PagerReview.prototype.initialize = function() {
        this.$el = $(this.el);
    };
    PagerReview.prototype.render = function() {
        this.$el.empty();
        this.$el.html(this.template(this.pages));
        this.bindEvents();
        return this;
    };
    PagerReview.prototype.bindEvents = function() {
        this.$el.find('.review-to-page').unbind('click').bind('click', this.toPage.bind(this));
    };
    PagerReview.prototype.toPage = function(e) {
        var page = $(e.currentTarget).attr('data-page');
        if (page) {
            this.trigger('changedPage', page);
        }
    };
    PagerReview.prototype.on = function(eventName, cb, contaxt) {
        $(this).on(eventName, cb.bind(contaxt));
    };
    PagerReview.prototype.trigger = function(eventName, data) {
        $(this).trigger(eventName, data);
    };



    // Main Level
    function RatingAndReview(pOptions) {
        this.query = {};
        this.el = pOptions.el;
        this.template = getTemplate('template-review-main');
        this.initialize();
    }
    RatingAndReview.prototype.initialize = function() {
        this.$el = $(this.el);
        this.subView = new ModelCollectionView({
            el: '#rating-review-block',
            query: this.query
        });
        this.render();
    };
    RatingAndReview.prototype.render = function() {
        this.$el = $(this.el);
        this.$el.empty();
        this.$el.hide();
        this.$el.html(this.template());
        this.subView.render();
        this.subView.collection.on('fetched', this.afterSubRender, this);
        this.bindEvents();
        return this;
    };
    RatingAndReview.prototype.afterSubRender = function() {
        this.pagerView = new PagerReview({
            pages: this.subView.collection.pages,
            el: '.review-rating-pager'
        }).render();
        this.pagerView.on('changedPage', function(e, _page) {
            this.query.page = _page;
            this.initialize();
        }, this);
        this.$el.show();
    };
    RatingAndReview.prototype.bindEvents = function() {
        this.subView.bindEvents();
        this.$el.find('#btn-write-a-new-review').unbind('click').bind('click', this.toggleAddNew.bind(this));
        this.$el.find('#frm-review-new-block').unbind('submit').bind('submit', this.addNewReview.bind(this));
        this.$el.find('#sel-query-review').unbind('change').bind('change', this.updatedSelectBox.bind(this));
        this.$el.find('#review-prev-page').unbind('click').bind('click', this.toPrevPage.bind(this));
        this.$el.find('#review-next-page').unbind('click').bind('click', this.toNextPage.bind(this));
    };
    RatingAndReview.prototype.updatedSelectBox = function(e) {
        // console.log($(e.currentTarget).val());
        //TODO::: use values of sorting specs
        this.render();
    };
    RatingAndReview.prototype.toggleAddNew = function() {
        this.$el.find('.review-new-block-wrap').toggle();
    };
    RatingAndReview.prototype.addNewReview = function(e) {
        e.preventDefault();
        // this.subView.collection.addNew($(e.currentTarget).serializeObject());
        $(e.currentTarget)[0].reset();
    };
    RatingAndReview.prototype.toNextPage = function(e) {
        e.preventDefault();
        this.query.page = this.query.page ? this.query.page + 1 : 1;
        this.render();
    };
    RatingAndReview.prototype.toPrevPage = function(e) {
        e.preventDefault();
        this.query.page = this.query.page ? this.query.page - 1 : 1;
        this.render();
    };

    (new RatingAndReview({
        el: '.review-rating'
    }));

})(jQuery, Drupal, Backbone, Handlebars, HandlebarsIntl);


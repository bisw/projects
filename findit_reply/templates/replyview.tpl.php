<?php
/**
 *
 */
?>
    <style>
    .review-img-wrap {
        float: left;
        width: 20%;
    }

    .review-body-wrap {
        float: right;
        width: 80%;
    }

    #rating-review-block img {
        height: 50px;
        width: 50px;
    }

    #rating-review-block li {
        display: inline-block;
        width: 100%;
    }

    .review-stars {
        float: left;
        margin: 1em;
    }

    .review-date {
        float: left;
    }

    .review-toggle-section div {
        float: left;
        margin: 1em;
    }

    .reply-toggle-section div {
        float: left;
        margin: 1em;
    }
    </style>
    <div class="review-rating">
    </div>
    <script id="template-review-main-thread" type="text/x-handlebars-template">
        <div class="review-img-wrap"><img src="{{image}}" alt="{{name}}"></img>{{name}}
        </div>
        <div class="review-body-wrap">
            <div class="title">{{subject}}</div>
            <div class="review-stars">{{rating}}
                <div id="star-five"></div>
                <div id="star-five"></div>
                <div id="star-five"></div>
                <div id="star-five"></div>
                <div id="star-five"></div>
            </div>
            <date class="review-date">{{formatDate created day="numeric" month="long" year="numeric"}}</date>
            <div class="review-social-icons"></div>
            <div class="review-main-body">
                <p>{{description}}</p>
                <div class="review-posted-imgs">
                    {{#each attacted_images}}
                    <img src="{{thumbnail}}" alt=""> {{/each}}
                </div>
            </div>
            <div class="review-toggle-section">
                <div class="review-comments">{{children_total}} Comment</div>
                <div class="review-helpful">Was this helpful to you?</div>
                <div class="review-help-yes">yes( {{like}}) </div>
                <div class="review-help-no">no( {{dislike}}) </div>
                <div class="review-report-abuse">Report Abuse ({{report }}) </div>
               {{#if canPublish}} <div class="review-publish-unpublish">{{#if status}} Unpublish{{else}} Publish {{/if}} </div>{{/if}}
            </div>
            <div class="review-reply-body-wrap" style="display:none;">
                <ul class="review-reply-body-{{id}}"></ul>
                <form id="frm-review-reply-input">
                    <input type="text" placeholder="Write a comment" name="description">
                    <button>SUBMIT</button>
                </form>
            </div>
        </div>
    </script>
    <script id="template-review-reply-thread" type="text/x-handlebars-template">
        <img src="{{image}}" alt="{{name}}"> {{name}}
        <date class="review-date">{{formatDate created day="numeric" month="long" year="numeric"}}</date>
        <p>{{description}}</p>
        <div class="reply-toggle-section">
            <div class="reply-helpful">Was this helpful to you?</div>
            <div class="reply-help-yes">yes( {{like}} )</div>
            <div class="reply-help-no">no( {{dislike}})</div>
            <div class="reply-report-abuse">Report Abuse ({{report }})</div>
            {{#if canPublish}}
            <div class="review-publish-unpublish">{{#if status}} Unpublish{{else}} Publish {{/if}} </div>{{/if}}
        </div>
    </script>
    <script id="template-review-main" type="text/x-handlebars-template">
        <div>
            <div class="review-new-block-wrap" style="display:none;">
                <form id="frm-review-new-block" action="">
                    <input type="text" name="title">
                    <button>Submit</button>
                </form>
            </div>
            <!--<div id="btn-write-a-new-review"> Write a review</div>-->
            <select id="sel-query-review" class="form-control">
                <option value="1" selected="selected">Most Recent</option>
                <option value="2">Most Popular</option>
                <option value="3">Most Review</option>
            </select>
            <ul id="rating-review-block"></ul>
            <ul class="review-rating-pager">
            </ul>
        </div>
    </script>
    <script id="template-review-pager" type="text/x-handlebars-template">
        {{#if hasPrev}}
        <li><a data-page="{{prev}}" class="review-to-page" href="#">&laquo;</a></li>{{/if}} {{#if hasNext}}
        <li><a data-page="{{next}}" class="review-to-page" href="#">&raquo;</a></li>{{/if}}
    </script>

/* Copyright (c) 2013 Chad Tetreault
 * http://palebanana.com - @chadtatro
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ==================================================
 *      GLOBALS
 * ==================================================
 */

App = {};
Settings = {};
b64Cache = [];

filtering = false;
thePhoto = document.getElementById('the-photo');
thePhoto.style.height = window.innerWidth + 'px';

var theCanvas = document.getElementById('canvas');
theCanvas.style.width = window.innerWidth;
theCanvas.style.height = window.innerWidth;



/* ==================================================
 *      HANDLE NATIVE CHIT-CHAT
 * ==================================================
 */

try {
    navigator.cascades.onmessage = function onmessage(message) {
        message = JSON.parse(message);

        // load photo
        if (message.cmd === 'loadPhoto') {
            thePhoto.src = 'data:image/png;base64,' + message.data;
            prepareImage();

            // save photo
        } else if (message.cmd === 'savePhoto') {
            savePhoto();

            // filter the image
        } else if (message.cmd === 'filter') {
            if (!filtering) {
                setTimeout(function() {
                    filtering = true;
                    process(message.data);
                }, 350);
            } else {
                return;
            }
        }
    };
} catch (e) {}



/* ==================================================
 *      PREPARE BASE64 FOR SAVING
 * ==================================================
 */

var savePhoto = function() {
    var b64 = thePhoto.src;
    navigator.cascades.postMessage(b64);
};



/* ==================================================
 *      PROCESS FILTERS
 * ==================================================
 */

var process = function(effect) {
    filterId = effect.id;

    // normal filter, reset the image source
    if (effect.id === 'normal') {
        thePhoto.src = b64Cache['normal'];
        navigator.cascades.postMessage('filter-done');
        filtering = false;
        return;

    } else {

        // if filter already applied and cached, change the image source to cahced base64 data
        if (b64Cache[effect.id]) {
            thePhoto.src = b64Cache[effect.id];
            filtering = false;
            setTimeout(function() {
                navigator.cascades.postMessage('filter-done');
            }, 250);

            // filter not cached, filter the image now
        } else {

            // build the curves array if needed
            if (effect.filter.curves) {
                var a = effect.filter.curves.a;
                var r = effect.filter.curves.r;
                var g = effect.filter.curves.g;
                var b = effect.filter.curves.b;

                a = JSON.parse("[" + a + "]");
                r = JSON.parse("[" + r + "]");
                g = JSON.parse("[" + g + "]");
                b = JSON.parse("[" + b + "]");

                curvesObject = {
                    "a": a[0],
                    "r": r[0],
                    "g": g[0],
                    "b": b[0]
                };

                // not using curves for this filter, set to false
            } else {
                curvesObject = false;
            }

            // build the filter object which we send to filter.me
            var filter = {
                curves: curvesObject,
                desaturate: effect.filter.desaturate,
                vignette: effect.filter.vignette
            };

            // process the filter
            jQuery(document).ready(function($) {
                $('#the-photo').filterMe(
                    filter
                );

                // 'done filtering' callback
                $(this).bind('filterMe.processEnd', function(event, base) {
                    filtering = false;
                    b64Cache[filterId] = thePhoto.src;
                    navigator.cascades.postMessage('filter-done');
                });
            });
        }
    }
};



/* ==================================================
 *      LOAD, RESCALE, AND CREATE THE PHOTO
 * ==================================================
 */

var prepareImage = function(filepath, success, error) {
    setTimeout(function() {
        var canvas = jQuery('canvas').get(0);
        var ctx = canvas.getContext('2d');

        // we want a square image
        canvas.width = window.innerWidth;
        canvas.height = window.innerWidth;
        ctx.drawImage(thePhoto, 0, 0, canvas.width, canvas.height);

        // clear the cache if it exists, and set the 'normal' image data
        b64Cache = [];
        b64Cache['normal'] = canvas.toDataURL('image/png');

        // set the photo source
        thePhoto.src = b64Cache['normal'];
    }, 100);
};
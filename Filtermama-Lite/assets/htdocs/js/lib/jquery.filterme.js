/*
 * jQuery filter.me is Dual licensed under the MIT (http://www.opensource.org/licenses/mit-license.php) and GPL (http://www.opensource.org/licenses/gpl-license.php) licenses.
 * Copyright © 2012 Matthew Ruddy (http://matthewruddy.com).
 *
 * @author Matthew Ruddy
 * @version 1.0
 */

(function($) {

    /*
     * Main plugin object
     */
    $.filterMe = function(el, options) {

        var base = this,
            filter,
            o;

        // Store the jQuery element and create canvas element
        base.$el = $(el);
        base.canvas = document.createElement('canvas');
        base.ctx = base.canvas.getContext('2d');
        base.url = '';

        /*
         * Initiate filter
         */
        base._init = function() {

            // Get the filter (from the images data-filter attribute)
            //filter = $.filterMe.filters[base.$el.data('filter')] || false;
            filter = options || false;

            // Get the options
            o = base.options = $.extend({}, $.filterMe.defaults, filter);

            // Trigger actions
            base.$el.trigger('fitlerIO._init', base);

            // Begin the process
            base.process();

        };

        /*
         * Processes and image and applies the effects
         */
        base.process = function() {

            // Check for canvas support before continuing
            if (!base.canvas.getContext)
                return;

            // Let's go!
            var image = new Image();

            // Get image src and load it
            // image.src = base.$el.attr('src');
            image.src = b64Cache['normal'] || base.$el.attr('src');

            image.onload = function() {

                // Set the canvas dimensions
                base.canvas.width = window.innerWidth;
                base.canvas.height = window.innerWidth;

                // Draw the image onto the canvas
                base.ctx.drawImage(this, 0, 0, this.width, this.height, 0, 0, base.canvas.width, base.canvas.height);

                // Trigger beginning action
                base.$el.trigger('filterMe.processBegin', base);

                // Apply curves effect
                if (o.curves)

                    base.addCurves();

                // Apply desaturation
                if (o.desaturate)
                    base.deSaturate(o.desaturate);

                // Apply vignette effect
                if (o.vignette)
                    base.addVignette();

                // Get the output URL globally (for ease of access)
                base.url = base.outputURL();

                // Output the image!
                base.outputImage();

                // Processing finished action
                base.$el.trigger('filterMe.processEnd', base);

            };
        };

        /*
         * Does a desaturation
         */
        base._doSaturation = function(saturation) {

            // Saturation fallback
            saturation = saturation || 1;

            // Get image data
            var imageData = base.ctx.getImageData(0, 0, base.canvas.width, base.canvas.height),
                data = imageData.data,
                length = data.length,
                average;

            // Apply the desaturation
            for (var i = 0; i < length; i += 4) {
                average = (data[i] + data[i + 1] + data[i + 2]) / 3;
                data[i] += (Math.round(average - data[i] * saturation));
                data[i + 1] += (Math.round(average - data[i + 1] * saturation));
                data[i + 2] += (Math.round(average - data[i + 2] * saturation));
            }

            // Restore modified image data
            imageData.data = data;
            return imageData;
        };

        /*
         * Applies a desaturation
         */
        base.deSaturate = function(saturation) {

            // Do the desaturation
            var imageData = base._doSaturation(saturation);

            // Put the image data
            base.putImageData(imageData);
        };

        /*
         * Does a curves adjustment
         */
        base._doCurves = function() {

            var curves,
                i;

            // Get the curves
            curves = o.curves;

            // Get the canvas image data
            var imageData = base.ctx.getImageData(0, 0, base.canvas.width, base.canvas.height),
                data = imageData.data,
                length = data.length;

            // Apply the color R, G, B values to each individual pixel
            for (i = 0; i < length; i += 4) {
                data[i] = curves.r[data[i]];
                data[i + 1] = curves.g[data[i + 1]];
                data[i + 2] = curves.b[data[i + 2]];
            }

            // Apply the overall RGB contrast changes to each pixel
            for (i = 0; i < length; i += 4) {
                data[i] = curves.a[data[i]];
                data[i + 1] = curves.a[data[i + 1]];
                data[i + 2] = curves.a[data[i + 2]];
            }

            // Restore modified image data
            imageData.data = data;
            return imageData;
        };

        /*
         * Applies a curves adjust (aka the filter) to the image
         */
        base.addCurves = function() {

            // Apply the adjustments
            var imageData = base._doCurves();

            // Put the image data
            base.putImageData(imageData);
        };

        /*
         * Adds a vignette effect to the image
         */
        base.addVignette = function() {

            var gradient,
                outerRadius = Math.sqrt(Math.pow(base.canvas.width / 2, 2) + Math.pow(base.canvas.height / 2, 2));

            // Adds outer darkened blur effect
            base.ctx.globalCompositeOperation = 'source-over';
            gradient = base.ctx.createRadialGradient(base.canvas.width / 2, base.canvas.height / 2, 0, base.canvas.width / 2, base.canvas.height / 2, outerRadius);
            gradient.addColorStop(0, 'rgba(0, 0, 0, 0)');
            gradient.addColorStop(0.65, 'rgba(0, 0, 0, 0)');
            gradient.addColorStop(1, 'rgba(0, 0, 0, 0.6)');
            base.ctx.fillStyle = gradient;
            base.ctx.fillRect(0, 0, base.canvas.width, base.canvas.height);

            // Adds central lighten effect
            base.ctx.globalCompositeOperation = 'lighter';
            gradient = base.ctx.createRadialGradient(base.canvas.width / 2, base.canvas.height / 2, 0, base.canvas.width / 2, base.canvas.height / 2, outerRadius);
            gradient.addColorStop(0, 'rgba(255, 255, 255, 0.1)');
            gradient.addColorStop(0.65, 'rgba(255, 255, 255, 0)');
            gradient.addColorStop(1, 'rgba(0, 0, 0, 0)');
            base.ctx.fillStyle = gradient;
            base.ctx.fillRect(0, 0, base.canvas.width, base.canvas.height);

        };

        /*
         * Applies the image data (for example, after pixel maniupulation)
         */
        base.putImageData = function(imageData) {
            base.ctx.putImageData(imageData, 0, 0);
        };

        /*
         * Outputs the image URL
         */
        base.outputURL = function() {
            var url = base.canvas.toDataURL('image/png');
            base.$el.trigger('filterMe.outputURL', url);
            return url;
        };

        /*
         * Outputs the image to the original Image src
         */
        base.outputImage = function() {
            base.$el.trigger('filterMe.outputImage');
            base.$el.attr('src', base.url);
        };

        // Store the data for external usages
        base.$el.data('filterMe', base);

        // Let's begin!
        base._init();

    };

    /*
     * Sets the filters
     */
    $.filterMe.filters = {};

    /*
     * Default plugin options
     */
    $.filterMe.defaults = {
        desaturate: false,
        curves: false,
        vignette: false,
        debug: false
    };

    /*
     * jQuery plugin
     */
    $.fn.filterMe = function(options) {
        return this.each(function() {
            new $.filterMe(this, options);
        });
    };

})(jQuery);
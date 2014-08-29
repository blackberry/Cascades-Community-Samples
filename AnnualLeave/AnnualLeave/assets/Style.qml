/*
 * Copyright (c) 2014 BlackBerry Limited.
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

import bb.cascades 1.2

Container {
    // title bars
    property variant title_bar_button_bg_color: Color.create("#093c91")
    property variant title_bar_label_color: Color.White
    property variant title_bar_label_font_size: FontSize.Small

	// inbox
    property variant inbox_empty_color: Color.create("#00d3f9")
    
    // requests
    property variant details_text_color: Color.create("#00d3f9")
    property variant request_label_style: FontWeight.Bold

	// tasks
    property variant task_label_style: FontWeight.Bold

	// about page
    property variant about_label_style: FontWeight.Bold

    // help page
    property variant help_label_style: FontWeight.Bold

	// settings page
    property variant settings_label_style: FontWeight.Bold
    
    // misc
    property variant error_text_color: Color.Red
    property variant heading_weight: FontWeight.W800
    

}

/* Copyright (c) 2012 Research In Motion Limited.
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

import bb.cascades 1.0

//This basic list implementation just hands the ListView the DataModel then
// lets it decide how to display it to the end user. Not a great end-UI, but 
// not a bad guess without any direction.

Page {
    content: Container {
        background: Color.create(0.4, 0.5, 0.2)
        ListView {
            dataModel: XmlDataModel {
                source: "../models/samplenominees.xml"
            }
        }
    }
}

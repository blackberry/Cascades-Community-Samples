/* Copyright (c) 2014 BlackBerry.
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

/*
 * Datsource component is for the RSS list
 */

import bb.cascades 1.2
import bb.data 1.0

// A neat trick is that we can load data from a RSS feed by specifying a query, and then we get XML.

DataSource {
    id: ds
    property variant dataModel

    // Set up a query to request the items in the RSS xml file
    query: "/rss/channel/item" //a query needs to be present
    source: ""

    onDataLoaded: {
        // When data is received clear the data model that is going to be populated
        dataModel.clear();

        // The data need some preparation so we create an array for holding the modified data.
        var tempdata = new Array();
        for (var i = 0; i < data.length; i ++) {

            // Set the data to temp data, we need to do this in order to modify the vaules
            // it is not possible to directly change the data array.
            tempdata[i] = data[i]

            // An invokable function in the C++ app code is used to parse the
            // data and look for an image that can be presented together with the text.
            tempdata[i].imageSource = application.findImage(data[i]);
            tempdata[i].description = data[i].description;
        }

        // Finally insert the data in the dataModel and it will be presented in the list.
        dataModel.insertList(tempdata)
    }

    onError: {
        console.log("rss channel load error[" + errorType + "]: " + errorMessage)
    }

    //Now we want other data.
    onSourceChanged: {
        dataModel.clear();
        ds.load();
    }

}

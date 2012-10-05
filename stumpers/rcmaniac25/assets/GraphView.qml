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
import com.stump.graph 1.0

Container {
    id: graphContainer
    leftPadding: 50
    rightPadding: leftPadding
    topPadding: leftPadding
    bottomPadding: leftPadding
    
    layout: DockLayout {
    }
    
    // A custom control for presenting custom data, see
    // data/graph.dat and GraphControl.cpp for further detail.
    GraphControl {
        id: graph
        graphDataSource: "data/graph.dat"
        
        // The preferred width and height has to be set in the current implementation.
        preferredWidth: 1280 - graphContainer.leftPadding * 2
        preferredHeight: 768 - graphContainer.leftPadding * 2
    }    
    
    onTouch: {
        // When the user release the finger on the Graph a new data set
        // is loaded.
        if (event.isUp()) {
            if (graph.graphDataSource == "data/graph3.dat") {
                graph.graphDataSource = "data/graph.dat"
            } else if (graph.graphDataSource == "data/graph2.dat") {
                graph.graphDataSource = "data/graph3.dat"
            } else {
                graph.graphDataSource = "data/graph2.dat"
            }
        }
    }
}

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

//This class will use the TaskControl custom component (see TaskControl.qml)


Page {
    content: Container {
        background: Color.create ("#BEE554")
        
        //This TextField will get updated with the changes to any tasks
        TextField {
            id: updateField
        }
        TaskControl {
            taskName: "Get Milk"
            //When the task changes (CheckBox is toggled) we can capture the event here
            onTaskStatusChanged: {
                updateField.text = taskName + " - " + (completed ? "Completed!" : "Not Completed")
            }
        }
        TaskControl {
            taskName: "Run 20km"
            completed: true
            //When the task changes (CheckBox is toggled) we can capture the event here
            onTaskStatusChanged: {
                updateField.text = taskName + " - " + (completed ? "Completed!" : "Not Completed")
            }
        }
        TaskControl {
            taskName: "Finish Webcast"
            //When the task changes (CheckBox is toggled) we can capture the event here
            onTaskStatusChanged: {
                updateField.text = taskName + " - " + (completed ? "Completed!" : "Not Completed")
            }
        }
    }
}

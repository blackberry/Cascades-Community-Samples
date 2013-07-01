// Default empty project template
import bb.cascades 1.0

Page {
    property string searchString
    onSearchStringChanged: {
        // Show the search UI if it's not visible
        
        // Do the search
        
    }
    
    // This is just an example UI for you to test with
    Container {
        TextField {
            id: searchField
            text: searchString
            hintText: "Enter search string"
            onTextChanging: {
                searchString = text;
            }
        }
        Label {
            text: "Searching for: " + searchString
        }

        Label {
            
            text: searchString.length > 0 ? "No '" + searchString + "' found." : ""
        }
    }
}


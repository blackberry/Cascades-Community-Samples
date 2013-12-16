import bb.cascades 1.0

Page {
    Container {
        background: Color.Black
        layout: DockLayout {
        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Carousel {
            maxHeight: 600
            minHeight: 600
            verticalAlignment: VerticalAlignment.Center
            
            // Autoscrolling properties
            autoScrollEnabled: true
            autoScrollInterval: 5000
            
            list: [ "asset:///images/1.jpg", 
            		"asset:///images/2.jpg", 
            		"asset:///images/3.jpg", 
            		"asset:///images/4.jpg",
                	"asset:///images/5.jpg" ]
            onCreationCompleted: {
                setIndex(0)
            }    
        }
    }
}


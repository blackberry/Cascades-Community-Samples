// Default empty project template
import bb.cascades 1.0
//Media Player control Import
import bb.multimedia 1.0

// creates one page with a label
Page {
    //Attached Objects
   attachedObjects: [

        MediaPlayer {
                    //ID of media player
                    id: myPlayer
                    //Source of where your media is selected from
                    //We will use a drop down for this
                    sourceUrl: soundSelectorDropdown.selectedValue
                    
                     
                }
    ]
    //Main Container
    Container {
        //Container for dropdown
        Container {
        layout: StackLayout {
            orientation: LayoutOrientation.TopToBottom
        }
        //Center all components on page
        horizontalAlignment: HorizontalAlignment.Center
        //Create our drop down to hold selections
        DropDown {
            id: soundSelectorDropdown //Id that goes in sourceURL
            title: "Sounds"
               //Next Create List of sounds
               Option {
                                text : "Fan 1"
                                description : "" //This is optional component
                                value : "asset:///music/fan1.wav" //Where the file located
                       }
                  Option {
                                   text : "Fan 2"
                                   description : ""
                                   value : "asset:///music/fan2.wav"
                          }        
        } //End Drop Down
    }//End Drop Down Container
    
    //Media Player Controls
    Container {
            //We want these side by side
            //So we use a stacklayout orientation left to right
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            horizontalAlignment: HorizontalAlignment.Center
            topPadding: 20.0 //Added to give some space
            ImageButton {//Play Button
                                  id: systemSoundPlayButton //
                                  defaultImageSource: "images/play_btn1.png" //Source where image button is
                                  onClicked: {
                                  myPlayer.play() //Calls our media player and gives command of Play
                                                              }
                      minHeight: 100.0
                  } // End Play Button
                  ImageButton { //Pause Button
                                  id: customSoundPlayButton2
                                  defaultImageSource: "images/pause_btn1.png" //Source where image button is
                                  onClicked: {
                                  myPlayer.pause() //Calls our media player and gives command of Pause
                                  }
                                  minHeight: 100.0
                              }//End Pause Button
                  ImageButton {
                                   id: customSoundPlayButton3
                                   defaultImageSource: "images/stop_btn1.png" //Source where image button is
                                   onClicked: {
                                   myPlayer.stop() //Calls our media player and gives command of Pause
                                   }
                                  minHeight: 100.0
                              } //End Stop Button
        
    }//End Media Player Controls Container
    } //End Main Container
}


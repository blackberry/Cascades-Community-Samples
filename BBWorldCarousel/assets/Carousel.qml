import bb.cascades 1.0
import my.library 1.0

Container {
    id: coRootCarousel
    property variant list
    property int index: 0
    property string direction: ""
    property bool autoScrollEnabled: true
    property alias autoScrollInterval: tAutoScroll.interval
    property int scrollSpeed: 300
    background: Color.Black    
    layout: DockLayout {
    }
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    function setIndex(_index) {
        index = _index
        coImages.controls[0].image = (index > 0 ? list[index - 1] : list[list.length - 1]);
        coImages.controls[1].image = (list[index]);
        coImages.controls[2].image = (index < list.length - 1 ? list[index + 1] : list[0])
        tAutoScroll.start()
    }
    function animateLeft(){
        coRootCarousel.direction = "left"
        coTouchHandler.enabled = false
        coTouchHandler.xInitial = -1     
        
        coImages.controls[2].animation1.toX = 0
        coImages.controls[2].animationDuration =  calculateAnimationDuration(coImages.controls[2], coImages.controls[2].animation1)   
        coImages.controls[2].animation1.play()
        
        coImages.controls[1].animation2.toX = coImages.wSup / -2
        coImages.controls[1].animationDuration =  coImages.controls[2].animationDuration   
        coImages.controls[1].animation2.play()
    }
    function animateRight(){
        coRootCarousel.direction = "right"
        coTouchHandler.enabled = false
        coTouchHandler.xInitial = -1
        
        coImages.controls[1].animation2.toX = coImages.wSup
        coImages.controls[1].animationDuration =  calculateAnimationDuration(coImages.controls[1], coImages.controls[1].animation2)
        coImages.controls[1].animation2.play()
        
        coImages.controls[0].animation1.toX = 0
        coImages.controls[0].animationDuration =  coImages.controls[1].animationDuration   
        coImages.controls[0].animation1.play()
        
        coImages.controls[2].translationX = coImages.wSup
    }   
    function finishLeftTransition() {
        if (index < list.length - 1) index ++;
        else index = 0
        coImages.controls[0].image = (index > 0 ? list[index - 1] : list[list.length - 1]);
        coImages.controls[1].image = (index < list.length - 1 ? list[index + 1] : list[0])
        coImages.controls[1].visible = false
        coImages.controls[1].translationX = coImages.wSup
        coImages.swap(2, 1)
        coImages.controls[2].visible = true
        coTouchHandler.enabled = true
    }
    function finishRightTransition() {
        if (index > 0) index --;
        else index = list.length - 1
        coImages.controls[1].image = (index > 0 ? list[index - 1] : list[list.length - 1]);
        coImages.controls[2].image = (index < list.length - 1 ? list[index + 1] : list[0])
        coImages.swap(0, 1)
        coImages.controls[0].translationX = coImages.wSup / -2
        coImages.controls[0].coOpaco.opacity == 1
        coTouchHandler.enabled = true
    }
    function calculateAnimationDuration(control, animation){
        var duration = Math.abs(control.translationX - animation.toX) * scrollSpeed / coImages.wSup
        return duration
    }
    attachedObjects: [
        OrientationHandler {
            onOrientationAboutToChange: {
                coImages.controls[0].translationX = coImages.hSup / -2
                coImages.controls[2].translationX = coImages.hSup
            }
            onOrientationChanged: {
                coImages.controls[0].translationX = coImages.wSup / -2
                coImages.controls[2].translationX = coImages.wSup
            }
        },
        QTimer{
            id: tAutoScroll
            interval: 5000
            onTimeout:{
                if(autoScrollEnabled)
                	animateLeft()
            }
        }
    ]
    Container {
        implicitLayoutAnimationsEnabled: false
        id: coImages
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        property variant hSup
        property variant wSup

        onHSupChanged: {
            console.log("Height: " + hSup)
        }
        onWSupChanged: {
            console.log("Width: " + wSup)
        }
        attachedObjects: [
            LayoutUpdateHandler {
                onLayoutFrameChanged: {
                    coImages.hSup = layoutFrame.height
                    coImages.wSup = layoutFrame.width
                }
            }
        ]
        layout: DockLayout {
        }
        Carousellmage {
            translationX: coImages.wSup / -2
            onCreationCompleted: {
                coOpaco.opacity == 1
            }
        }
        Carousellmage {
            translationX: 0
        }
        Carousellmage {
            translationX: coImages.wSup
        }
    }
    Container {
        property int xInitial: 0
        property int xCurrent: 0
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        id: coTouchHandler       
        onTouch: {
            if(!enabled)
            	return
            if (event.isMove()) {
                if(autoScrollEnabled)
                	tAutoScroll.stop()
                xCurrent = event.localX < 0 ? 0 : (event.localX > coImages.wSup - 1 ? coImages.wSup - 1 : event.localX)
                if (xInitial == -1) xInitial = xCurrent
                //console.log("Touch event | current X= " + xCurrent + " started in " + xInitial)
                if (xCurrent < xInitial) {
                    coImages.controls[1].translationX = (xCurrent - xInitial) / 2
                    coImages.controls[2].translationX = coImages.wSup + (xCurrent - xInitial)
                    coImages.controls[0].translationX = coImages.wSup / -2
                } else if (xCurrent > xInitial) {
                    coImages.controls[0].translationX = (coImages.wSup / -2) + (xCurrent - xInitial) / 2
                    coImages.controls[1].translationX = (xCurrent - xInitial)
                    coImages.controls[2].translationX = coImages.wSup
                }
            } else if (event.isDown()) {
                if(autoScrollEnabled)
                	tAutoScroll.stop()
                xInitial = event.localX
                xCurrent = xInitial                
                //console.log("Touch start in X= " + xInitial)
            } else if (event.isUp() || event.isCancel()) {
                if(autoScrollEnabled)
                	tAutoScroll.start()
                //console.log("Touch event | current X= " + xCurrent + " started in " + xInitial)
                if (xCurrent < xInitial - (coImages.wSup / 10)) {
                    animateLeft()                    
                } else if (xCurrent > xInitial + (coImages.wSup / 10)) {
                    animateRight()
                } else {
                    coTouchHandler.enabled = false
                    coRootCarousel.direction = "cancel"
                    coImages.controls[0].animation2.toX = coImages.wSup / -2
                    coImages.controls[0].animationDuration = 100   
                    coImages.controls[0].animation2.play()
                    
                    coImages.controls[1].animation2.toX = 0
                    coImages.controls[1].animationDuration = 100   
                    coImages.controls[1].animation2.play()
                    
                    coImages.controls[2].animation2.toX = coImages.wSup
                    coImages.controls[2].animationDuration = 100
                    coImages.controls[2].animation2.play()
                }
            }
        }
        implicitLayoutAnimationsEnabled: false
    }
}
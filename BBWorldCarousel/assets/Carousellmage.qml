import bb.cascades 1.0

Container {
    implicitLayoutAnimationsEnabled: false
    preferredHeight: coImages.hSup
    preferredWidth: coImages.wSup
    property alias image: image.imageSource
    property variant coOpaco: coFx
    property alias animation1: animation1
    property alias animation2: animation2
    property alias animationHide: hide
    property int animationDuration: coRootCarousel.scrollSpeed
    layout: DockLayout {
    }
    visible: translationX != coImages.wSup
    id: coImage
    onTranslationXChanged: {
        if(coImages.indexOf(coImage) == 1 || coImages.indexOf(coImage) == 0){
            coFx.opacity =  translationX * 100 / (coImages.wSup * 100 / -2)        
        }
    }    
    ImageView {
        id: image
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFill
        implicitLayoutAnimationsEnabled: false
        
    }
    Container {
        id: coFx
        opacity: 0.0
        background: Color.Black
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        animations: [
            FadeTransition {
                id: hide
                toOpacity: 0
                duration: animationDuration
                easingCurve: StockCurve.SineOut
            },
            FadeTransition {
                id: obscure
                toOpacity: 1
                duration: animationDuration
                easingCurve: StockCurve.SineOut
                onEnded: {
                   coFx.opacity = 0
                }
            }
        ]
    }
    animations: [
        TranslateTransition {
            id: animation1            
            easingCurve: StockCurve.SineOut
            onEnded: {
                if (coRootCarousel.direction == "left") {
                    coRootCarousel.finishLeftTransition()
                } else if (coRootCarousel.direction == "right") {
                    coRootCarousel.finishRightTransition()
                }
            }
            onStarted: {
                coImage.visible = true
                if (coRootCarousel.direction == "right") {
                    hide.play()
                }
            }
            duration: animationDuration
        },
        TranslateTransition {
        	id: animation2
            easingCurve: StockCurve.SineOut
            onStarted: {
                if (coRootCarousel.direction == "left") {
                    obscure.play()
                }
            }
            onEnded: {
                if (coRootCarousel.direction == "cancel") {
                	coTouchHandler.enabled = true
                }
            }
            duration: animationDuration
        }
        
    ]
}

import QtQuick 2.5
import CustomElements 1.0

CustomTouchArea {
    secondTouchEnabled: false

    property real initialPinchDistance: 1.0
    property real initialScale: 1.0
    property Item plane
    property real minScale: 0.2
    property real maxScale: 2.0
    property real lastEventScale: 1.0

    KineticEffect2D {
        id: kineticEffect
        friction: 0.06
        onMoving: {
            plane.x = position.x
            plane.y = position.y
        }
    }

    function onExternalPositionChange() {
        kineticEffect.stopMovement()
        kineticEffect.setValue(plane.x, plane.y)
    }

    Timer {
        // set value of KineticEffect to inital value:
        running: true
        repeat: false
        interval: 10
        onTriggered: {
            kineticEffect.setValue(plane.x, plane.y)
        }
    }

    onTouchDown: {
        if (zoomOutInAnimation.running) return
        planeMinScaleAnimation.stop()
        planeNormalScaleAnimation.stop()
        if (!getSecondTouch().isValid) {
            kineticEffect.start(touch.x, touch.y)
        } else {
            //initialPinchDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            initialScale = plane.scale
            // console.log("Pinch started. Initial dist: ", initialPinchDistance)
        }
    }

    onTouchMove: {
        if (zoomOutInAnimation.running) return
        if (!getSecondTouch().isValid) {
            kineticEffect.update(touch.x, touch.y)
        } else {
            //var currentDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            //var scale = currentDistance / initialPinchDistance
            //if (scale > minScale) {
            //    plane.scale = scale * initialScale
            //}
        }
    }

    onTouchUp: {
        if (zoomOutInAnimation.running) return
        if (!getSecondTouch().isValid) {
            kineticEffect.stop(touch.x, touch.y)
            if (plane.scale < initialScale) {
                planeMinScaleAnimation.start()
            } else {
                planeNormalScaleAnimation.start()
            }
        }
    }

    NumberAnimation {
        id: planeMinScaleAnimation
        target: plane
        property: "scale"
        duration: 500
        easing.type: Easing.OutCubic
        to: minScale
    }

    NumberAnimation {
        id: planeNormalScaleAnimation
        target: plane
        property: "scale"
        duration: 500
        easing.type: Easing.OutBounce
        to: 1.0
    }

    SequentialAnimation {
        id: zoomOutInAnimation
        PauseAnimation { duration: 200 }
        NumberAnimation {
            target: plane
            properties: "customScale"
            duration: 500
            easing.type: Easing.OutCubic
            from: 1.0
            to: 0.2
        }
        PauseAnimation { duration: 500 }
        NumberAnimation {
            target: plane
            properties: "customScale"
            duration: 800
            easing.type: Easing.InCubic
            from: 0.2
            to: 1.0
        }
        onStopped: controller.blockManager().setHideBlocksOutsideViewports(true)
    }

    onDoubleClick: {
        if (zoomOutInAnimation.running) return
        controller.projectManager().centerViewOnBlocks()
        controller.blockManager().setHideBlocksOutsideViewports(false)

        plane.scaleOriginX = -plane.x + plane.width / 2
        plane.scaleOriginY = -plane.y + plane.height / 2
        zoomOutInAnimation.start()
    }

    onScrollEvent: {
        plane.x += deltaX
        plane.y += deltaY
        onExternalPositionChange()
        scrollEventWasAccepted()
    }
}

import QtQuick 2.5
import CustomElements 1.0

TouchArea {
    minimumTouchPoints: 1
    maximumTouchPoints: 2
    property real initialPinchDistance: 1.0
    property real initialScale: 1.0
    property Item plane
    property real minScale: 0.5
    property real maxScale: 2.0
    property real lastEventScale: 1.0

	KineticEffect2D {
		id: kineticEffect
        friction: 0.1
        onMoving: {
			plane.x = position.x
			plane.y = position.y
        }
	}

	function onExternalPositionChange() {
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
        planeMinScaleAnimation.stop()
        planeNormalScaleAnimation.stop()
        if (secondTouch === false) {
			kineticEffect.start(touch.sceneX, touch.sceneY)
        } else {
            initialPinchDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            initialScale = plane.scale
            // console.log("Pinch started. Initial dist: ", initialPinchDistance)
        }
    }

    onTouchMove: {
		if (secondTouch === false) {
			kineticEffect.update(touch.sceneX, touch.sceneY)
        } else {
            var currentDistance = Math.sqrt(Math.pow(touch.sceneX - firstTouch.sceneX, 2) + Math.pow(touch.sceneY - firstTouch.sceneY, 2))
            var scale = currentDistance / initialPinchDistance
            if (scale > minScale) {
                plane.scale = scale * initialScale
            }
        }
    }

    onTouchUp: {
		if (secondTouch === false) {
			kineticEffect.stop(touch.sceneX, touch.sceneY)
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
}

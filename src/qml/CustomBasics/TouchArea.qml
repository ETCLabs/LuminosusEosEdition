import QtQuick 2.0

MultiPointTouchArea {
    mouseEnabled: true
    maximumTouchPoints: 1
    property bool pressed: false
    property var firstTouch: false
    property var secondTouch: false
    signal touchDown(TouchPoint touch)
    signal touchMove(TouchPoint touch)
	signal touchUp(TouchPoint touch, bool canceled)

    onGestureStarted: {
		// gesture.grab()
		// console.log("Gesture started.")
    }

    onPressed: {
        var touch = touchPoints[0]
        if (firstTouch === false) {
            firstTouch = touch
        } else if (secondTouch === false) {
            secondTouch = touch
        } else {
            return
        }
        pressed = true
        touchDown(touch)
    }

    onUpdated: {
        touchMove(touchPoints[0])
    }

    onReleased: {
        var touch = touchPoints[0]
        if (touch !== firstTouch && touch !== secondTouch) {
            return
        }
		touchUp(touch, false)
        if (touch === firstTouch) {
            firstTouch = secondTouch
            secondTouch = false
            if (!firstTouch) pressed = false
        } else if (touch === secondTouch) {
            secondTouch = false
        }
    }

	onCanceled: {
        pressed = false
        var touch = touchPoints[0]
        if (touch !== firstTouch && touch !== secondTouch) {
            return
        }
		touchUp(touch, true)
        if (touch === firstTouch) {
            firstTouch = secondTouch
            secondTouch = false
            if (!firstTouch) pressed = false
        } else if (touch === secondTouch) {
            secondTouch = false
        }
    }
}


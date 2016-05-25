import QtQuick 2.5
import CustomElements 1.0


TouchArea {
	id: scrollArea
	anchors.fill: parent
	clip: true
	property Item contentItem

	KineticEffect {
		id: kineticEffect
		minValue: Math.min(0, scrollArea.height - contentItem.height - 20*dp)
		maxValue: 0
		onMoving: contentItem.y = position
	}

	onTouchDown: {
		kineticEffect.start(touch.y)
	}

	onTouchMove: {
		kineticEffect.update(touch.y)
	}

	onTouchUp: {
		kineticEffect.stop(touch.y)
	}

	// --------------------- Handle Mouse Wheel -------------------

	MouseArea {
		anchors.fill: parent
		acceptedButtons: Qt.NoButton

		onWheel: {
			var delta = 0.0;
			if (wheel.pixelDelta.y) {
				delta = wheel.pixelDelta.y
			} else {
				delta = wheel.angleDelta.y / 3
			}
			contentItem.y = Math.max(kineticEffect.minValue, Math.min(contentItem.y + delta, kineticEffect.maxValue))
			kineticEffect.setValue(contentItem.y)
		}
	}
}

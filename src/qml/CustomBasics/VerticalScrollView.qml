import QtQuick 2.5
import CustomElements 1.0


CustomTouchArea {
	id: scrollArea
	anchors.fill: parent
    clip: contentItem.y < 0
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

    onScrollEvent: {
        contentItem.y = Math.max(kineticEffect.minValue, Math.min(contentItem.y + deltaY, kineticEffect.maxValue))
        kineticEffect.setValue(contentItem.y)
        scrollEventWasAccepted()
    }
}

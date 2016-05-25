import QtQuick 2.5
import QtGraphicalEffects 1.0

Rectangle {
	id: root
    property var block
    property bool pressed: false
    property bool focused: false
    property Component settingsComponent: noSettings

	onXChanged: block.positionChanged()
	onYChanged: block.positionChanged()


	// background color of all blocks:
	color: "#444"

	// shadow behind block:
	RectangularGlow {
		id: effect
		width: parent.width
		height: parent.height
		y: 3*dp
		z: -1
		glowRadius: pressed ? 12*dp : 4*dp
		Behavior on glowRadius {
			NumberAnimation {
				duration: 400
				easing.type: Easing.OutBounce
			}
		}
		spread: 0.1
		//color: focused ? Qt.rgba(0.1, 0.4, 0.8, 0.5) : Qt.rgba(0, 0, 0, 0.5)
		//color: focused ? Qt.rgba(0.9, 0.8, 0.0, 0.5) : Qt.rgba(0, 0, 0, 0.5)
		color: Qt.rgba(0, 0, 0, 0.5)
		cornerRadius: glowRadius
		visible: GRAPHICAL_EFFECTS_LEVEL >=2
	}

	// focus border:
	RectangularGlow {
		anchors.fill: parent
		z: -1
		glowRadius: 10*dp
		spread: 0.1
		color: Qt.rgba(0.9, 0.8, 0.0, 0.5)
		cornerRadius: glowRadius
		visible: focused
    }

	Component {
		id: noSettings
		Item {
			height: 60*dp

			Text {
				anchors.centerIn: parent
				text: "No settings available\nfor " + block.getBlockName() + "."
				color: "#fff"
				horizontalAlignment: Text.AlignHCenter
				font.family: "Quicksand"
				font.pixelSize: 16*dp
			}
		}
	}

	function moveAnimatedTo(newX, newY) {
		xAnimator.from = x
		xAnimator.to = newX
		yAnimator.from = y
		yAnimator.to = newY
		xAnimator.start()
		yAnimator.start()
	}

	NumberAnimation {
		id: xAnimator
		target: root
		property: "x"
		duration: 500
		easing.type: Easing.OutCubic
		onStopped: {
			block.positionChangedExternal()
		}
	}
	NumberAnimation {
		id: yAnimator
		target: root
		property: "y"
		easing.type: Easing.OutCubic
		duration: xAnimator.duration
	}

	// ------------------ Keyboard Handling ----------------------

	// Delete Key:
	Keys.onDeletePressed: {
		if (focused) {
			controller.blockManager().deleteFocusedBlock()
			event.accepted = true
		}
	}

	// D Key and Ctrl + C:
	Keys.onPressed: {
		if (!focused) return;
		if (event.key === Qt.Key_D && (event.modifiers & Qt.ControlModifier)) {
			controller.blockManager().duplicateFocusedBlock()
			event.accepted = true
		} else if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
			controller.blockManager().copyFocusedBlock()
			event.accepted = true
		}
	}

}


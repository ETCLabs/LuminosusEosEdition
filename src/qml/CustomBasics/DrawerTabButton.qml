import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../CustomBasics"

TouchArea {
	id: root
	implicitWidth: -1

	property bool active: false
	property bool toggle: false
	property bool marked: false
	property string iconName: ""

	signal press

	Rectangle {
		anchors.fill: parent
		color: Qt.rgba(0.5, 0.7, 1, 1)
		opacity: 0.3
		visible: marked
	}

	TouchFeedbackEffect {
		id: touchFeedbackEffect
		startWhen: root.pressed
	}

	Image {
		id: icon
		source: "qrc:/images/" + iconName + ".png"
		width: Math.min(parent.height, parent.width) - 16*dp
		height: width
		anchors.centerIn: parent
		smooth: true
	}

	onTouchDown: {
		controller.checkForExternalInputConnection(uid)
		if (toggle) {
			active= !active
		} else {
			active = true
		}
		if (active) {
			press()
		}
	}

	onTouchUp: {
		if (!toggle) {
			active = false
		}
	}

	property string uid: ""
	property real externalInput: 0
	Component.onCompleted: controller.registerGuiControl(this)
	Component.onDestruction: if (controller) controller.unregisterGuiControl(uid)
	onExternalInputChanged: {
		if (externalInput > 0.) {
			active = true
			touchFeedbackEffect.start()
		} else {
			active = false
		}
	}
}


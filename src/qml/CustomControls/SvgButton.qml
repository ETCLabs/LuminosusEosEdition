import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
	id: root
	property bool active: false
	property bool toggle: false
	property string iconName: ""
	property bool inverted: false
    property alias size: icon.width
    property alias rotation: icon.rotation
	//C:\Users\thenning\Documents\breeze_icons\actions

    signal press

    mouseOverEnabled: true
    Rectangle {
        id: mouseOverFeedback
        anchors.fill: parent
        color: "white"
        opacity: 0.05
        visible: parent.mouseOver
    }

	TouchFeedbackEffect {
		id: touchFeedbackEffect
		startWhen: root.pressed
	}

    Image {
        id: icon
        width: root.height - 8*dp
        height: width
        anchors.centerIn: parent
        source: "qrc:/images/svg/" + iconName + ".svg"
        fillMode: Image.PreserveAspectFit
        smooth: true
    }

	Rectangle {
		id: bottomLine
		color: "#2C89E1"
		height: 2*dp
		width: Math.min(parent.height, parent.width) - 8*dp
		y: parent.height - height
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Rectangle {
		property real ratio: parent.active ? 1 : 0
		color: "yellow"
		height: 2*dp
		width: bottomLine.width * ratio
		y: parent.height - height
		anchors.horizontalCenter: parent.horizontalCenter
		Behavior on ratio {
		   NumberAnimation { duration: 100 }
	   }
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
    Component.onDestruction: if (controller) controller.unregisterGuiControl(this)
	onExternalInputChanged: {
		if (externalInput > 0.) {
			active = true
			touchFeedbackEffect.start()
		} else {
			active = false
		}
	}
}


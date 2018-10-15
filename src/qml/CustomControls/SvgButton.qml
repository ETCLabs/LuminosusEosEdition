import QtQuick 2.5
import QtQuick.Window 2.0
import CustomStyle 1.0
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

        // render in double resolution to be antialiased and smooth:
        sourceSize.width: width * Screen.devicePixelRatio * 2
    }

	Rectangle {
		id: bottomLine
        color: Style.primaryActionColor
		height: 2*dp
		width: Math.min(parent.height, parent.width) - 8*dp
		y: parent.height - height
		anchors.horizontalCenter: parent.horizontalCenter
	}

	Rectangle {
		property real ratio: parent.active ? 1 : 0
        color: Style.primaryHighlightColor
		height: 2*dp
		width: bottomLine.width * ratio
		y: parent.height - height
		anchors.horizontalCenter: parent.horizontalCenter
		Behavior on ratio {
		   NumberAnimation { duration: 100 }
	   }
	}

	onTouchDown: {
        controller.playClickSound()
        controller.midiMapping().guiControlHasBeenTouched(mappingID)
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

    // ------------------------- External Input ------------------------

	property string mappingID: ""
    property real externalInput: -1
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
	onExternalInputChanged: {
		if (externalInput > 0.) {
			active = true
			touchFeedbackEffect.start()
            press()
		} else {
			active = false
		}
	}
    onActiveChanged: controller.midiMapping().sendFeedback(mappingID, active ? 1.0 : 0.0)
}


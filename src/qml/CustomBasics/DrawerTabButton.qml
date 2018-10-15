import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
	id: root
	implicitWidth: -1

	property bool active: false
	property bool toggle: false
	property bool marked: false
	property string iconName: ""

	signal press

	Rectangle {
		anchors.fill: parent
        color: Style.primaryActionColor
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

	property string mappingID: ""
	property real externalInput: 0
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
	onExternalInputChanged: {
		if (externalInput > 0.) {
			active = true
			touchFeedbackEffect.start()
		} else {
			active = false
		}
	}
}


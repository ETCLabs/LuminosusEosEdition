import QtQuick 2.5
import CustomElements 1.0

import "../CustomBasics"

CustomTouchArea {
	id: root
	// stretch in both directions:
	implicitWidth: -1
	implicitHeight: -1

    secondTouchEnabled: true

	// public properties:
	property bool active: false
	property bool pressed: false
	property bool toggle: false
	property bool marked: false
	property string text: ""
    property bool showLed: true

	// emitted when active changes to true
	signal press

	// ------------------------- Visuals -------------------------

	BorderImage {
		anchors.fill: parent
		border { left: 23; top: 23; right: 23; bottom: 23 }
		horizontalTileMode: BorderImage.Stretch
		verticalTileMode: BorderImage.Stretch
		source: pressed ? "qrc:/images/push_button_down_70.png" : "qrc:/images/push_button_normal_70.png"
		visible: dp <= 1.5
	}

	BorderImage {
		anchors.fill: parent
		border { left: 46; top: 46; right: 46; bottom: 46 }
		horizontalTileMode: BorderImage.Stretch
		verticalTileMode: BorderImage.Stretch
		source: pressed ? "qrc:/images/push_button_down_140.png" : "qrc:/images/push_button_normal_140.png"
		visible: dp > 1.5
	}

	Image {
		width: 40*dp
		height: width * (sourceSize.height / sourceSize.width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: active ? 10*dp : 12*dp
		source: "qrc:/images/push_button_led_off.png"
        visible: showLed
	}

	Image {
		width: 40*dp
		height: width * (sourceSize.height / sourceSize.width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: active ? 10*dp : 12*dp
		source: "qrc:/images/push_button_led_blue.png"
        visible: marked && showLed
	}

	Image {
		width: 40*dp
		height: width * (sourceSize.height / sourceSize.width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: active ? 10*dp : 12*dp
		source: "qrc:/images/push_button_led_yellow.png"
		opacity: active ? 1 : 0
		Behavior on opacity {
			NumberAnimation {
				duration: 300
				easing.type: Easing.OutCubic
			}
		}
        visible: showLed
	}

	Text {
        height: parent.height - 16*dp
        width: parent.width - 16*dp
		anchors.centerIn: parent
        anchors.verticalCenterOffset: showLed ? (pressed ? -4*dp : -6*dp) : (pressed ? -2*dp : -4*dp)
		style: Text.Raised
		styleColor: "#222"
		color: "#999"
		font.bold: true
		fontSizeMode: Text.Fit
        wrapMode: Text.Wrap  // break into new line if too long
		horizontalAlignment: Text.AlignHCenter
		text: root.text
		visible: root.text !== ""
	}

	// ------------------------- Touch Handling -------------------------

	onTouchDown: {
		controller.checkForExternalInputConnection(uid)
		handleTouchDown()
	}

	function handleTouchDown() {
		if (toggle) {
			active = !active
		} else {
			active = true
		}
		pressed = true
		if (active) {
			press()
		}
	}

	onTouchUp: {
        if (!getFirstTouch().isValid) {
            handleTouchUp()
        }
	}

	function handleTouchUp() {
		if (!toggle) {
			active = false
		}
		pressed = false
	}

	// ------------------------- External Input ------------------------

	property string uid: ""
	property real externalInput: 0
	Component.onCompleted: controller.registerGuiControl(this)
    Component.onDestruction: if (controller) controller.unregisterGuiControl(this)
	onExternalInputChanged: {
		if (externalInput > 0.) {
			handleTouchDown()
		} else {
			handleTouchUp()
		}
	}

}

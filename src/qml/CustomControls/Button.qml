import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
    id: root
	height: 30*dp
	implicitWidth: -1
    secondTouchEnabled: true
    property bool active: false
    property alias text: text_area.text
    property alias fontSize: text_area.font.pixelSize
    property bool allUpperCase: true
    property bool toggle: false
    property alias text_area: text_area
    property alias color: text_area.color

    signal press

    TouchFeedbackEffect {
        id: touchFeedbackEffect
        startWhen: root.pressed
    }

    Text {
        id: text_area
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "lightgrey"
        font.pixelSize: 16*dp
		font.capitalization: allUpperCase ? Font.AllUppercase : Font.MixedCase
        font.family: "Quicksand"
        font.bold: true
        fontSizeMode: Text.Fit
        elide: Text.ElideRight
        maximumLineCount: 1
    }

    onTouchDown: {
        controller.checkForExternalInputConnection(uid)
        if (toggle) {
			active = !active
        } else {
            active = true
        }
        if (active) {
            press()
        }
    }

    onTouchUp: {
        if (!toggle && !getFirstTouch().isValid) {
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


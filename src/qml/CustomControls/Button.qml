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
    property bool wrap: false
    property bool sendActiveMidiFeedback: true

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
        maximumLineCount: wrap ? 3 : 1
        wrapMode: wrap ? Text.WrapAnywhere : Text.NoWrap
    }

    onTouchDown: {
        controller.blockManager().playClickSound()
        controller.midiMapping().guiControlHasBeenTouched(mappingID)
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

    // ---------------------- External Input Handling ----------------------

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
    onActiveChanged: if (sendActiveMidiFeedback) controller.midiMapping().sendFeedback(mappingID, active ? 1.0 : 0.0)
}


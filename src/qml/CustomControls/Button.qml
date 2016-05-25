import QtQuick 2.0
import "../CustomBasics"

TouchArea {
    id: root
	height: 30*dp
	implicitWidth: -1
    property bool active: false
    property alias text: text_area.text
    property alias fontSize: text_area.font.pixelSize
    property bool allUpperCase: true
    property bool toggle: false
    property alias text_area: text_area

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
        if (!toggle) {
            active = false
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        enabled: false
        onContainsMouseChanged: {
            if (containsMouse) {
                mouseLeaveAnimation.stop()
                backgroundRectangle.opacity = 0.1
            } else {
                mouseLeaveAnimation.restart()
            }
        }
        onClicked: {
            mouse.accepted = false
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


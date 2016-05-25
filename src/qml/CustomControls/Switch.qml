import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../CustomBasics"

TouchArea {
    id: root
	height: 30*dp
	implicitWidth: -1
    property bool active: false
    property real markerOffset: active ? 1 : 0

    TouchFeedbackEffect {
        id: touchFeedbackEffect
        startWhen: root.pressed
    }

    Behavior on markerOffset {
        NumberAnimation {
            duration: 400
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        width: 14*dp
        height: 14*dp
        radius: 7*dp
        border.width: 2*dp
        border.color: "lightgrey"
        color: "transparent"
        x: (parent.width / 2) - 22*dp
        y: (parent.height / 2) - (height / 2)
    }

    Rectangle {
        width: 2*dp
        height: 14*dp
        color: "#2C89E1"
        x: (parent.width / 2) + 14*dp
        y: (parent.height / 2) - (height / 2)
    }

    Rectangle {
        width: 30*dp
        height: 24*dp
        radius: 5*dp
        border.color: active ? "#2C89E1" : "lightgrey"
        Behavior on border.color {
            ColorAnimation {}
        }

        border.width: 2*dp
        x: (parent.width / 2) - (width * (1-markerOffset))
        y: 3*dp
        color: "transparent"
    }

    onTouchDown: {
        controller.checkForExternalInputConnection(uid)
        active = !active
    }

    property string uid: ""
    property real externalInput: 0
    property real lastExternalInput: 0
    Component.onCompleted: controller.registerGuiControl(this)
    Component.onDestruction: if (controller) controller.unregisterGuiControl(uid)
    onExternalInputChanged: {
        if ((lastExternalInput < 0.5) && (externalInput > 0.5)) {
            active = !active
        }
        touchFeedbackEffect.start()
        lastExternalInput = externalInput
    }
}


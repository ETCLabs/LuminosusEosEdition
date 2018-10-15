import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
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
        antialiasing: dp <= 1
        border.width: 2*dp
        border.color: "lightgrey"
        color: "transparent"
        x: (parent.width / 2) - 22*dp
        y: (parent.height / 2) - (height / 2)
    }

    Rectangle {
        width: 2*dp
        height: 14*dp
        color: Style.primaryActionColor
        x: (parent.width / 2) + 14*dp
        y: (parent.height / 2) - (height / 2)
    }

    Rectangle {
        width: 30*dp
        height: 24*dp
        radius: 5*dp
        antialiasing: dp <= 1
        border.color: active ? Style.primaryActionColor : "lightgrey"
        Behavior on border.color {
            ColorAnimation {}
        }

        border.width: 2*dp
        x: (parent.width / 2) - (width * (1-markerOffset))
        y: 3*dp
        color: "transparent"
    }

    onClick: {
        controller.playClickSound()
        controller.midiMapping().guiControlHasBeenTouched(mappingID)
        active = !active
    }

    // ------------------------- External Input ------------------------

    property string mappingID: ""
    property real externalInput: 0
    property real lastExternalInput: 0
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
    onExternalInputChanged: {
        if ((lastExternalInput < 0.5) && (externalInput > 0.5)) {
            active = !active
        }
        touchFeedbackEffect.start()
        lastExternalInput = externalInput
    }
    onActiveChanged: controller.midiMapping().sendFeedback(mappingID, active ? 1.0 : 0.0)
}


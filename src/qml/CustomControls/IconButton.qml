import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../CustomBasics"

TouchArea {
    id: root
    property bool active: false
    property bool toggle: false
    property string iconName: ""
    property bool inverted: false
	//C:\Users\thenning\Documents\breeze_icons\actions

    signal press

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
        visible: false
        smooth: true
    }

    ColorOverlay {
        anchors.fill: icon
        source: icon
        color: inverted ? "#000" : "#00000000"
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


import QtQuick 2.5
import QtGraphicalEffects 1.0
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
    id: root
    property bool active: false
    property bool toggle: false
    property string iconName: ""
	//C:\Users\thenning\Documents\breeze_icons\actions

    signal press

    TouchFeedbackEffect {
        id: touchFeedbackEffect
        startWhen: root.pressed
    }

    // ------------------------ Icon Image -------------------------

    Image {
        id: icon
        source: "qrc:/images/" + iconName + ".png"
        width: Math.min(parent.height, parent.width) - 16*dp
        height: width
        anchors.centerIn: parent
        smooth: true
    }

    // ------------------ Blue / Yellow Line at the bottom -----------------------

    Rectangle {
        color: Style.primaryActionColor
        height: 1*dp
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 6*dp
        anchors.rightMargin: 6*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 12*dp
    }

    Rectangle {
        property real ratio: parent.active ? 1 : 0
        color: Style.primaryHighlightColor
        height: 1*dp
        width: (parent.width - 12*dp) * ratio
        x: 6*dp + (1 - ratio) * (parent.width / 2 - 6*dp)
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 12*dp
        Behavior on ratio {
           NumberAnimation { duration: 100 }
       }
    }

    // ---------------------------- Logic ------------------------------------

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

    // ---------------------- External Input Handling ----------------------

    property string mappingID: ""
    property real externalInput: -1
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
    onActiveChanged: controller.midiMapping().sendFeedback(mappingID, active ? 1.0 : 0.0)
}


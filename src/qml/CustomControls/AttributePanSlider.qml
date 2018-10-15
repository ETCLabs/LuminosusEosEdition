import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
    id: root
    implicitWidth: -1  // stretch width by default

    // public attributes:
    property QtObject attr
    property int padding: 5*dp
    property real value: attr.val
    onValueChanged: {
        if (value !== attr.val) {
            attr.val = value
            controller.midiMapping().sendFeedback(mappingID, value)
        }
    }


    Rectangle {  // grey BG line
        height: 2*dp
        color: "#666"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: padding
        anchors.right: parent.right
        anchors.rightMargin: padding
    }

    Rectangle {  // dot in the middle
        height: 8*dp
        width: 2*dp
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#666"
    }

    Rectangle {  // cursor
        height: 20*dp
        width: 2*dp
        anchors.verticalCenter: parent.verticalCenter
        x: padding + (((parent.width - padding * 2) / 2.0) + (value - 0.5) * (parent.width - padding * 2)) - 1*dp
        color: Style.primaryActionColor
    }

    // ------------------------- Touch Logic ------------------------------

    KineticEffect {
        id: kineticEffect
        friction: 0.18
        minValue: padding
        maxValue: parent.width - padding
        onMoving: setValueByPosition(position)
    }

    onTouchDown: {
        controller.midiMapping().guiControlHasBeenTouched(mappingID)
        kineticEffect.setValue(touch.itemX)
        kineticEffect.start(touch.itemX)
        setValueByPosition(touch.itemX)
    }

    onTouchMove: {
        kineticEffect.update(Math.max(kineticEffect.minValue, Math.min(touch.itemX, kineticEffect.maxValue)))
    }

    onTouchUp: {
        kineticEffect.stop(Math.max(kineticEffect.minValue, Math.min(touch.itemX, kineticEffect.maxValue)))
    }

    function setValueByPosition(position) {
        var val = Math.max(0.0, Math.min(1.0, (position - padding) / (width - 2*padding)))
        if (Math.abs(val - 0.5) < 0.1) {
            val  = 0.5
        }
        guiManager.setPropertyWithoutChangingBindings(root, "value", val)
        // "this" as first parameter does not work, bug in QML?
    }

    // ---------------------- External Input Handling ----------------------

    property string mappingID: attr.block().getUid() + attr.name()
    property real externalInput: 0
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
    onExternalInputChanged: {
        guiManager.setPropertyWithoutChangingBindings(this, "value", externalInput)
    }
}


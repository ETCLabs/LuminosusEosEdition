import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"


CustomTouchArea {
    id: root
    implicitHeight: -1  // stretch height by default

    // public attributes:
    property QtObject attr1
    property QtObject attr2
    property int padding: 20*dp


    Rectangle {  // grey BG line
        width: 2*dp
        color: "#666"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: padding
        anchors.bottom: parent.bottom
        anchors.bottomMargin: padding
    }

    Rectangle {  // cursor1
        id: cursor1
        width: 25*dp
        height: 5*dp
        radius: 3*dp
        color: Style.primaryActionColor
        anchors.horizontalCenter: parent.horizontalCenter
        y: Math.round(padding + (parent.height - 2*padding) * (1 - attr1.val) - height / 2)
    }

    Rectangle {  // cursor2
        id: cursor2
        width: 25*dp
        height: 5*dp
        radius: 3*dp
        color: Style.primaryActionColor
        anchors.horizontalCenter: parent.horizontalCenter
        y: Math.round(padding + (parent.height - 2*padding) * (1 - attr2.val) - height / 2)
    }

    // ------------------------- Touch Logic ------------------------------

    property int activeAttribute: 1

    onTouchDown: {
        var val = 1 - Math.max(0.0, Math.min(1.0, (touch.itemY - padding) / (height - 2*padding)))
        var diff1 = Math.abs(attr1.val - val)
        var diff2 = Math.abs(attr2.val - val)
        if (diff1 < diff2) {
            // change attribute 1:
            controller.midiMapping().guiControlHasBeenTouched(extInput1.mappingID)
            activeAttribute = 1
            attr1.val = val
        } else {
            // change attribute 2:
            controller.midiMapping().guiControlHasBeenTouched(extInput2.mappingID)
            activeAttribute = 2
            attr2.val = val
        }
    }

    onTouchMove: {
        var val = 1 - Math.max(0.0, Math.min(1.0, (touch.itemY - padding) / (height - 2*padding)))
        if (activeAttribute == 1) {
            // change attribute 1:
            attr1.val = val
        } else {
            // change attribute 2:
            attr2.val = val
        }
    }

    // ---------------------- External Input Handling ----------------------

    Item {
        id: extInput1
        property string mappingID: attr1.block().getUid() + attr1.name()
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            attr1.val = externalInput
        }
        Connections {
            target: attr1
            onValChanged: controller.midiMapping().sendFeedback(extInput1.mappingID, attr1.val)
        }
    }
    Item {
        id: extInput2
        property string mappingID: attr2.block().getUid() + attr2.name()
        property real externalInput: -1
        Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
        Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
        onExternalInputChanged: {
            attr2.val = externalInput
        }
        Connections {
            target: attr2
            onValChanged: controller.midiMapping().sendFeedback(extInput2.mappingID, attr2.val)
        }
    }
    Component.onCompleted: {
        controller.midiMapping().sendFeedback(extInput1.mappingID, attr1.val)
        controller.midiMapping().sendFeedback(extInput2.mappingID, attr2.val)
    }
}

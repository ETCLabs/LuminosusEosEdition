import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
    id: root
    implicitWidth: -1  // stretch width by default

    // public attributes:
    property int padding: 20*dp
    property real value: 0.0
    property real indicator: 0.0
    property bool useIndicator: false
    property bool midiMappingEnabled: true


    Rectangle {  // grey BG line
        height: 2*dp
        color: "#666"
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: padding
        anchors.right: parent.right
        anchors.rightMargin: padding
    }

    Rectangle {  // lightgrey BG line below cursor (if not useIndicator)
        height: 2*dp
        color: "#999"
        width: (parent.width - 2 * padding) * value
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: padding
        visible: !useIndicator
    }

    Rectangle {  // lightblue indicator line (if useIndicator)
        height: 2*dp
        color: Style.primaryActionColor
        width: (parent.width - 2 * padding) * indicator
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: padding
        visible: useIndicator
    }

    Image {  // cursor
        width: Math.round(32*dp)
        height: Math.round(32*dp)
        source: (dp <= 1) ? "qrc:/images/slider_cursor.png" : "qrc:/images/slider_cursor@2x.png"
        anchors.verticalCenter: parent.verticalCenter
        x: Math.round(padding + (parent.width - 2*padding) * value - width / 2)
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
        if (midiMappingEnabled) controller.midiMapping().guiControlHasBeenTouched(mappingID)
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
        guiManager.setPropertyWithoutChangingBindings(root, "value", val)
        // "this" as first parameter does not work, bug in QML?
    }

    // ---------------------- External Input Handling ----------------------

    property string mappingID: ""
    property real externalInput: -1
    Component.onCompleted: controller.midiMapping().registerGuiControl(this, mappingID)
    Component.onDestruction: if (controller) controller.midiMapping().unregisterGuiControl(mappingID)
    onExternalInputChanged: {
        guiManager.setPropertyWithoutChangingBindings(this, "value", externalInput)
    }
    onValueChanged: controller.midiMapping().sendFeedback(mappingID, value)
}

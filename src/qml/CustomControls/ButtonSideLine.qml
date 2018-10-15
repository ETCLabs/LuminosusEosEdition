import QtQuick 2.0
import CustomStyle 1.0
import "../CustomBasics"

Button {
    property bool marked: false
    sendActiveMidiFeedback: false
    onMarkedChanged: controller.midiMapping().sendFeedback(mappingID, marked ? 1.0 : 0.0)
    onActiveChanged: if (!active) controller.midiMapping().sendFeedback(mappingID, marked ? 1.0 : 0.0)

    mouseOverEnabled: true
    Rectangle {
        id: mouseOverFeedback
        anchors.fill: parent
        color: "white"
        opacity: 0.05
        visible: parent.mouseOver
    }

    Rectangle {
        color: marked ? Style.primaryHighlightColor : Style.primaryActionColor
        height: parent.height - 8*dp
        width: 2*dp
        x: 0
        y: 4*dp
    }

    Rectangle {
        property real ratio: parent.active ? 1 : 0
        color: Style.primaryHighlightColor
        height: 1*dp
        width: (parent.width - 16*dp) * ratio
        x: 8*dp + (1 - ratio) * (parent.width / 2 - 8*dp)
        y: parent.height - 6*dp
        Behavior on ratio {
           NumberAnimation { duration: 100 }
       }
    }

}

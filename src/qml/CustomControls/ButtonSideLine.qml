import QtQuick 2.0
import "../CustomBasics"

Button {
    property bool marked: false

    mouseOverEnabled: true
    Rectangle {
        id: mouseOverFeedback
        anchors.fill: parent
        color: "white"
        opacity: 0.05
        visible: parent.mouseOver
    }

    Rectangle {
        color: marked ? "yellow" : Qt.rgba(0.3, 0.5, 1, 0.7)
        height: parent.height - 8*dp
        width: 2*dp
        x: 0
        y: 4*dp
    }

    Rectangle {
        property real ratio: parent.active ? 1 : 0
        color: "yellow"
        height: 1*dp
        width: (parent.width - 16*dp) * ratio
        x: 8*dp + (1 - ratio) * (parent.width / 2 - 8*dp)
        y: parent.height - 6*dp
        Behavior on ratio {
           NumberAnimation { duration: 100 }
       }
    }

}

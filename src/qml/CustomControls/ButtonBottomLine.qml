import QtQuick 2.5
import CustomStyle 1.0
import "../CustomBasics"
import "../."

Button {

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

    // ------------------- Touch Feedback Effect ------------------------------

    mouseOverEnabled: true
    Rectangle {
        id: mouseOverFeedback
        anchors.fill: parent
        color: "white"
        opacity: 0.05
        visible: parent.mouseOver
    }
}

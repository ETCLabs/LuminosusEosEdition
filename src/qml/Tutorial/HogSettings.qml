import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    property var connection

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.top
        anchors.verticalCenterOffset: 60*dp
        horizontalAlignment: Text.AlignHCenter

        text: "1. Make sure all settings look like below
(except for the upper IP address)

2. Click 'OK'"
    }

    Image {
        anchors.fill: parent
        anchors.bottomMargin: 80*dp
        anchors.topMargin: 120*dp
        anchors.leftMargin: 20*dp
        anchors.rightMargin: 20*dp
        source: "qrc:/images/tutorial/hog/osc-settings.jpg"
        fillMode: Image.PreserveAspectFit
    }

    CustomTouchArea {
        width: 90*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        onClick: stackView.push("qrc:/qml/Tutorial/HogName.qml", { connection: connection })
        Text {
            anchors.centerIn: parent
            text: "Next ›"
            font.bold: true
            color: "white"
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.hsva(0, 0, 1, 0.5)
            visible: parent.mouseOver
        }
    }
}

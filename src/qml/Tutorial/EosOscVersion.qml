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

        text: "1. Scroll down in the Network section
in the Shell Settings
2. Enter the OSC Version below"
    }

    Image {
        anchors.fill: parent
        anchors.bottomMargin: 80*dp
        anchors.topMargin: 120*dp
        anchors.leftMargin: 20*dp
        anchors.rightMargin: 20*dp
		source: "qrc:/images/tutorial/eos/shell-osc.jpg"
        fillMode: Image.PreserveAspectFit
    }

    ComboBox2 {
        id: oscVersionInput
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20*dp
        width: 100*dp
        height: 40*dp
        values: [false, true]
        texts: ["OSC 1.0", "OSC 1.1"]
    }

    CustomTouchArea {
        width: 90*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        onClick: {
            connection.protocol = oscVersionInput.getValue() ? "TCP 1.1" : "TCP 1.0"
            stackView.push("qrc:/qml/Tutorial/EosShowSettings.qml", { connection: connection })
        }
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

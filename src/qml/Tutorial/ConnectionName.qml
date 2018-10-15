import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Text {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 80*dp
        anchors.rightMargin: 80*dp
        height: 120*dp
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: "How do you want to call this console?"
    }

    TextInput {
        id: consoleNameInput
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: 240*dp
        height: 40*dp
        hintText: "Console Name"
    }

    CustomTouchArea {
        width: 90*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        enabled: consoleNameInput.text.length >= 1
        onClick: stackView.push("qrc:/qml/Tutorial/EosOscVersion.qml")
        Text {
            anchors.centerIn: parent
            text: "Next ›"
            color: enabled ? "white" : "#aaa"
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.hsva(0, 0, 1, 0.5)
            visible: parent.mouseOver
        }
    }
}

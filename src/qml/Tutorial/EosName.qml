import QtQuick 2.10
import QtQuick.Controls 2.2
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    property var connection

    Text {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -50*dp

        text: "How do you want to call this console?"
    }

    TextInput {
        id: consoleNameInput
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 30*dp
        width: 160*dp
        height: 40*dp
        hintText: "Console Name"
    }

    CustomTouchArea {
        width: 200*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        enabled: consoleNameInput.text.length >= 1
        onClick: {
            Qt.inputMethod.commit()

            var name = consoleNameInput.text
            var ip = connection.ip
            var protocol = connection.protocol
            var udpTxPort = 8001
            var udpRxPort = 8001
            var tcpPort = 3032
            controller.lightingConsole().createAndLoadPreset("Eos", name, protocol, ip, udpTxPort, udpRxPort, tcpPort)

            var sV = stackView
            sV.pop(null, StackView.Immediate)  // null -> pop all except first
            sV.push("qrc:/qml/Tutorial/EosStatus.qml")
        }
        Text {
            anchors.centerIn: parent
            text: "Add this console ✓"
            font.bold: true
            color: enabled ? "white" : "#aaa"
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.hsva(0, 0, 1, 0.5)
            visible: parent.mouseOver
        }
    }
}

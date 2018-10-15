import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.top
        anchors.verticalCenterOffset: 60*dp
        horizontalAlignment: Text.AlignHCenter

        text: "1. Open the Shell Settings
2. Click on 'Network'
3. Enter the IP Address in the field below"
    }

    Image {
        anchors.fill: parent
        anchors.bottomMargin: 80*dp
        anchors.topMargin: 120*dp
        anchors.leftMargin: 20*dp
        anchors.rightMargin: 20*dp
		source: "qrc:/images/tutorial/eos/shell-ip.jpg"
        fillMode: Image.PreserveAspectFit
    }

    TextInput {
        id: ipInput
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20*dp
        width: 160*dp
        height: 40*dp
        hintText: "IP Address"
    }

    CustomTouchArea {
        width: 90*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        enabled: ipInput.text.length >= 7
        onClick: {
            Qt.inputMethod.commit()
            stackView.push("qrc:/qml/Tutorial/EosOscVersion.qml", { connection: { ip: ipInput.text } })
        }
        Text {
            anchors.centerIn: parent
            text: "Next ›"
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

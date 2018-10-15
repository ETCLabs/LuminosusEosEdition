import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Component.onCompleted: controller.eosManager().startDiscovery()
    Component.onDestruction: controller.eosManager().stopDiscovery()

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.top
        anchors.verticalCenterOffset: 60*dp

        text: "Looking for consoles on the network..."
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.leftMargin: 120*dp
        anchors.rightMargin: 120*dp
        anchors.topMargin: 120*dp
        anchors.bottomMargin: 120*dp
        spacing: 20*dp
        clip: true

        model: controller.eosManager().discoveredConsoles

        delegate: CustomTouchArea {
            width: listView.width
            height: 50*dp
            mouseOverEnabled: true

            onClick: {
                controller.eosManager().stopDiscovery()
                stackView.push("qrc:/qml/Tutorial/EosOscVersion.qml", { connection: { ip: modelData.ip } })
            }

            Rectangle {
                anchors.fill: parent
                color: "#444"
                border.color: parent.mouseOver ? "yellow" : "#aaa"
                border.width: 1
            }
            Text {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: 12*dp
                font.pixelSize: 16*dp
                text: modelData.ip
            }
            Text {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: -8*dp
                font.pixelSize: 20*dp
                font.bold: true
                text: modelData.name
            }
        }
    }

    CustomTouchArea {
        width: 90*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        onClick: {
            controller.eosManager().stopDiscovery()
            stackView.push("qrc:/qml/Tutorial/EosIp.qml")
        }
        Text {
            anchors.centerIn: parent
            text: "Manual ›"
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

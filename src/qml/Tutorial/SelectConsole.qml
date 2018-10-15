import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Text {
        text: "Select Your Console:"
        font.pixelSize: 26*dp
        font.bold: true
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -150*dp
        horizontalAlignment: Text.AlignHCenter
    }

    CustomTouchArea {
        width: 180*dp
        height: 50*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20*dp
        onClick: stackView.push("qrc:/qml/Tutorial/EosDiscovery.qml")
        mouseOverEnabled: true

        Rectangle {
            anchors.fill: parent
            color: "#444"
            border.color: parent.mouseOver ? "yellow" : "#aaa"
            border.width: 1
        }
        Text {
            anchors.centerIn: parent
            font.pixelSize: 22*dp
            text: "Eos Family"
        }
    }

    CustomTouchArea {
        width: 180*dp
        height: 50*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 60*dp
        onClick: stackView.push("qrc:/qml/Tutorial/HogIp.qml")
        mouseOverEnabled: true

        Rectangle {
            anchors.fill: parent
            color: "#444"
            border.color: parent.mouseOver ? "yellow" : "#aaa"
            border.width: 1
        }
        Text {
            anchors.centerIn: parent
            font.pixelSize: 22*dp
            text: "Hog 4"
        }
    }

}

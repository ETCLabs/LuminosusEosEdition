import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Text {
        text: "Welcome\nto Luminosus!"
        font.pixelSize: 26*dp
        font.bold: true
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -150*dp
        horizontalAlignment: Text.AlignHCenter
    }

    CustomTouchArea {
        width: 180*dp
        height: 80*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -20*dp
        onClick: tutorialView.open = false
        mouseOverEnabled: true

        Rectangle {
            anchors.fill: parent
            color: "#555"
            border.color: parent.mouseOver ? "yellow" : "#aaa"
            border.width: 1
        }
        Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: -15*dp
            font.pixelSize: 22*dp
            text: "Continue ›"
        }
        Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 15*dp
            font.pixelSize: 18*dp
            text: controller.projectManager().currentProjectName
            color: "#ccc"
            font.italic: true
        }
    }

    CustomTouchArea {
        width: 240*dp
        height: 50*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 80*dp
        onClick: stackView.push("qrc:/qml/Tutorial/SelectConsole.qml")
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
            text: "Connect a Console"
        }
    }

    CustomTouchArea {
        width: 240*dp
        height: 50*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 160*dp
        onClick: stackView.push("qrc:/qml/Tutorial/TutorialList.qml")
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
            text: "View Tutorials"
        }
    }

}

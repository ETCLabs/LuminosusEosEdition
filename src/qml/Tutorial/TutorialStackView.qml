import QtQuick 2.10
import QtQuick.Controls 2.2
import CustomElements 1.0

Item {
    id: root

    CustomTouchArea {
        anchors.fill: parent
        onClick: {}  // prevent click inside dialog from closing it
    }

    StackView {
        id: stackView
        anchors.fill: parent
        clip: true
        initialItem: MainOptions {}
    }

    CustomTouchArea {
        width: 40*dp
        height: 40*dp
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        onClick: stackView.pop()
        visible: stackView.depth > 1
        Image {
            width: 20*dp
            height: 10*dp
            anchors.centerIn: parent
            source: "qrc:/images/down_arrow_slim.png"
            rotation: 90
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.hsva(0, 0, 1, 0.5)
            visible: parent.mouseOver
        }
    }

    CustomTouchArea {
        width: 40*dp
        height: 40*dp
        anchors.right: parent.right
        mouseOverEnabled: true
        onClick: tutorialView.open = false
        Text {
            anchors.centerIn: parent
            text: "X"
            color: "white"
            font.bold: true
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.hsva(0, 0, 1, 0.5)
            visible: parent.mouseOver
        }
    }
}

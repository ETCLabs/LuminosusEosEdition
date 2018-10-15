import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../CustomBasics"
import "../CustomControls"

Rectangle {
    width: 1920
    height: 1080
    color: "black"

    property var block

    Item {
        anchors.centerIn: parent
        width: 670
        height: 670

        Image {
            anchors.fill: parent
            source: "qrc:/images/safe_lock_background.png"
        }

        Image {
            anchors.fill: parent
            source: "qrc:/images/safe_lock_foreground.png"
            transform: Rotation { origin.x: 670 / 2; origin.y: 670 / 2; angle: block.attr("angle").val}
        }
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 335
            height: 335
            radius: 167
            color: "transparent"
            border.width: 2
            border.color: block.attr("valid").val ? "green" : "red"
        }
    }
}

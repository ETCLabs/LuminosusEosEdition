import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../CustomBasics"
import "../CustomControls"

Rectangle {
    width: 1920
    height: 1080
    color: "black"
    opacity: 0

    property var block

    OpacityAnimator on opacity {
        id: showAnimation
        duration: 500
        to: 1
    }

    Component.onCompleted: showAnimation.start()

    AnimatedImage {
        anchors.fill: parent
        anchors.topMargin: 500
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/activity_monitor.gif"
    }

    AnimatedImage {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 150
        anchors.rightMargin: 150
        source: "qrc:/images/helix.gif"
        scale: 1.8
    }

    Text {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -230
        anchors.horizontalCenterOffset: -80

        color: "white"
        font.pixelSize: 65

        text: "Last symptoms: 2 days ago

Issue probability: low

Next doctor's appointment: Monday 2 pm"
    }
}

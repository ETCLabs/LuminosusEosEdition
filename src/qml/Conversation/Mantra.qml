import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../CustomBasics"
import "../CustomControls"

Rectangle {
    anchors.fill: parent
    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#f1cf94";
        }
        GradientStop {
            position: 1.00;
            color: "#ec923d";
        }
    }

    opacity: 0

    OpacityAnimator on opacity {
        id: showAnimation
        duration: 300
        to: 1
    }

    Component.onCompleted: showAnimation.start()

    property var block

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        y: 80*dp
        anchors.margins: 50*dp
        color: "#555"
        font.pixelSize: 50*dp
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        style: Text.Raised
        styleColor: "#777"
        wrapMode: Text.Wrap
        text: "Mantra"
    }

    Text {
        anchors.fill: parent
        anchors.margins: 50*dp
        color: "black"
        font.pixelSize: 30*dp
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        style: Text.Raised
        styleColor: "#777"
        wrapMode: Text.Wrap
        text: "I am sending all my compassion to Peter who is a victim like me, and a victim who suffers more than me."
    }

    CustomTouchArea {
        anchors.fill: parent
        onClick: block.hide()
    }

}

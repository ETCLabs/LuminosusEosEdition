import QtQuick 2.10
import QtQuick.Controls 2.2
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

Item {

    Text {
        text: "Done ✓"
        font.pixelSize: 36*dp
        font.bold: true
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -50*dp
        horizontalAlignment: Text.AlignHCenter
    }

    StretchRow {
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 10*dp
        width: implicitWidth
        height: 30*dp
        defaultSize: 1
        spacing: 10*dp
        Text {
            text: "Connection Status:"
        }
        Text {
            color: text === "Ok" ? "lightgreen" : Qt.hsva(0.0, 0.8, 1.0, 1.0)
            text: controller.eosManager().pingIsSuccessful
                  ? (controller.eosManager().latency < 200 ? "Ok" : "Slow")
                  : "Connecting..."
            font.bold: true
        }
    }

    Text {
        text: "Luminosus will automatically
connect to this console on startup.

You can remove this connection in the settings
and add new connections at any time."
        font.pixelSize: 18*dp
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 100*dp
        horizontalAlignment: Text.AlignHCenter
    }

    CustomTouchArea {
        width: 100*dp
        height: 40*dp
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        mouseOverEnabled: true
        onClick: {
            var sV = stackView
            sV.pop(null, StackView.Immediate)
            sV.push("qrc:/qml/Tutorial/TutorialList.qml")
        }
        Text {
            anchors.centerIn: parent
            text: "Tutorials ›"
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

import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 232*dp

    StretchColumn {
        anchors.fill: parent
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            text: "Master"
            onTouchDown: {
                if (touch.modifiers & Qt.ShiftModifier) {
                    block.sendUnloadEvent()
                } else {
                    block.sendLoadEvent(1.0)
                }
            }
            onTouchUp: {
                if (!(touch.modifiers & Qt.ShiftModifier)) {
                    block.sendLoadEvent(0.0)
                }
            }
            showLed: false
            mappingID: block.getUid() + "load"
            onExternalInputChanged: {
                if (externalInput <= 0) {
                    block.sendLoadEvent(0.0)
                } else  if (externalInput >= 1) {
                    block.sendLoadEvent(1.0)
                }
            }
        }
        Text {
            height: 30*dp
            text: "Last Fired:"
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
        }
        Text {
            height: 30*dp
            text: controller.eosManager().cueInfo
            horizontalAlignment: Text.AlignHCenter
            fontSizeMode: Text.Fit
        }
        Item {
            height: 2*dp
            Rectangle {
                height: parent.height
                color: controller.eosManager().activeCuePercentComplete < 1 ? "yellow" : "#2C89E1"
                width: parent.width * controller.eosManager().activeCuePercentComplete
            }
        }
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 40*dp
            onActiveChanged: block.sendStopEvent(active ? 1.0 : 0.0)
            showLed: false
            mappingID: block.getUid() + "stop"
            Image {
                width: 15*dp
                height: 15*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/fader_stop_icon.png"
            }
        }
        PushButton {
            implicitHeight: 0  // do not stretch
            height: 60*dp
            onActiveChanged: block.sendGoEvent(active ? 1.0 : 0.0)
            showLed: false
            mappingID: block.getUid() + "go"
            Image {
                width: 18*dp
                height: 18*dp
                anchors.centerIn: parent
                anchors.verticalCenterOffset: parent.pressed ? -2*dp : -4*dp
                source: "qrc:/images/fader_go_icon.png"
            }
        }

        DragArea {
            text: "Playback"
        }

    }  // end main Column
}

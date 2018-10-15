import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 165*dp
    height: 65*dp

    StretchColumn {
        anchors.fill: parent

        StretchText {
            id: clockLabel
            hAlign: Text.AlignHCenter
            font.family: "BPmono"
            font.pixelSize: 22*dp
            text: Qt.formatTime(new Date(), "hh:mm:ss")

            Timer {
               interval: 1000
               running: true
               repeat: true

               onTriggered: {
                   clockLabel.text = Qt.formatTime(new Date(), "hh:mm:ss")
               }
           }
        }

        DragArea {
            text: ""
        }
    }  // end main Column
}


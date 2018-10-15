import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 150*dp

    onWidthChanged: block.positionChanged()

    StretchColumn {
        anchors.fill: parent

        StretchText {
            id: timeText
            text: block.getTimeString()
            hAlign: Text.AlignHCenter
            font.family: "Courier"

            Connections {
                target: block
                onTimeStringChanged: timeText.text = block.getTimeString()
            }

            Timer {
                running: block.running
                repeat: true
                interval: 100
                onTriggered: timeText.text = block.getTimeString()
            }
        }
        BlockRow {
            InputNode {
                node: block.node("startInput")
                suggestions: ["Eos Cue"]
            }
            ButtonBottomLine {
                text: "Start"
                onPress: block.start()
                mappingID: block.getUid() + "start"
            }
        }
        BlockRow {
            InputNode {
                node: block.node("stopInput")
                suggestions: ["Eos Cue"]
            }
            ButtonBottomLine {
                text: "Pause"
                onPress: block.stop()
                mappingID: block.getUid() + "stop"
            }
        }
        BlockRow {
            InputNode {
                node: block.node("resetInput")
            }
            ButtonBottomLine {
                text: "Reset"
                onPress: block.reset()
                mappingID: block.getUid() + "reset"
            }
        }


        DragArea {
            text: "Stopwatch"
        }
    }
}


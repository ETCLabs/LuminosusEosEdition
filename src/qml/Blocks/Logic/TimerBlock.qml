import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            StretchText {
                id: remainingTime
                text: "00:00"
                hAlign: Text.AlignHCenter
                font.bold: true
                font.family: "Courier"
                color: block.attr("running").val ? "red" : "#666"
            }
            Timer {
                id: ledTimer
                repeat: true
                running: block.attr("running").val
                interval: 1000
                onTriggered: {
                    remainingTime.text = block.getRemainingTimeString()
                }
            }
        }

        DragArea {
            text: "Timer"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

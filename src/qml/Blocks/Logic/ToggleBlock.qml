import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 90*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                node: block.node("on")
            }
            StretchText {
                text: "On"
            }
        }
        BlockRow {
            InputNode {
                node: block.node("off")
            }
            StretchText {
                text: "Off"
            }
        }

        DragArea {
            text: "Toggle"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Eos Cue"]
            }
        }
    }
}

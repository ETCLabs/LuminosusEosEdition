import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 80*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            StretchText {
                text: "Join"
                hAlign: Text.AlignHCenter
            }
            OutputNode {
                node: block.node("outputNode2")
                suggestions: ["Eos Channel", "Vertical Patch"]
            }
        }

        DragArea {
            text: "Rows"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode1")
                suggestions: ["Eos Channel", "Vertical Patch"]
            }
        }
    }
}

import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 40*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                node: block.node("inputNode2")
                suggestions: ["Slider"]
            }
        }

        DragArea {
            text: "X"
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Slider"]
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

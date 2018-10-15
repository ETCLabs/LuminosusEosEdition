import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 180*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            height: 150*dp
            AttributeSlider {
                implicitWidth: -1
                attr: block.attr("offsetX")
            }
            AttributeSlider {
                implicitWidth: -1
                attr: block.attr("offsetY")
            }
        }

        DragArea {
            text: "Offset"
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Pixel Control"]
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

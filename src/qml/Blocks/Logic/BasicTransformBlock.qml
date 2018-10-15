import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 170*dp
    height: 90*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 8*dp
            StretchText {
                text: "Rotate"
            }
            AttributeCombobox {
                width: 90*dp
                attr: block.attr("rotate")
                values: [0, 45, 90, 180, 270]
                texts: ["0°", "45°", "90°", "180°", "270°"]
            }
        }

        BlockRow {
            leftMargin: 8*dp
            Text {
                width: 82*dp
                text: "Mirror V:"
            }
            AttributeCheckbox {
                width: 30*dp
                attr: block.attr("mirrorV")
            }
            Text {
                width: 20*dp
                text: "H:"
            }
            AttributeCheckbox {
                width: 30*dp
                attr: block.attr("mirrorH")
            }
        }

        DragArea {
            text: "Transform"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

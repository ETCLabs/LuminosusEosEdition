import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 110*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                node: block.node("inputX")
                suggestions: ["Slider", "Linear Value", "Random Value"]
            }
            StretchText {
                text: "Hue"
            }
        }
        BlockRow {
            InputNode {
                node: block.node("inputY")
                suggestions: ["Slider", "Sinus Value", "Random Value"]
            }
            StretchText {
                text: "Saturation"
            }
        }
        BlockRow {
            InputNode {
                node: block.node("inputZ")
                suggestions: ["Slider", "Sinus Value", "Random Value"]
            }
            StretchText {
                text: "Intensity"
            }
        }

        DragArea {
            text: "HSV"
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

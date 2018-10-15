import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 75*dp
    height: 200*dp

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            implicitHeight: -1
            StretchColumn {
                implicitWidth: 0
                width: 15*dp
                InputNode {
                    node: block.node("inputNode2")
                }
                Item {
                    implicitHeight: -0.5
                }
                InputNode {
                    node: block.node("ratioNode")
                }
                Item {
                    implicitHeight: -0.5
                }
                InputNode {
                    node: block.node("inputNode")
                }
            }
            StretchColumn {
                implicitWidth: -1
                ButtonBottomLine {
                    text: "A"
                    onPress: block.attr("ratio").val = 1
                }
                AttributeSlider {
                    attr: block.attr("ratio")
                }
                ButtonBottomLine {
                    text: "B"
                    onPress: block.attr("ratio").val = 0
                }
            }
        }

        DragArea {
            text: "X-fade"
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

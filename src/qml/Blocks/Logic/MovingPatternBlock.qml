import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 60*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 3*dp
            Text {
                width: 50*dp
                verticalAlignment: Text.AlignVCenter
                text: "Amount:"
            }
            AttributeNumericInput {
                implicitWidth: -1
                attr: block.attr("amount")
                decimals: 2
            }
        }

        DragArea {
            text: "-"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

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
            AttributeCombobox {
                width: 90*dp
                attr: block.attr("pattern")
                values: [0, 1]
                texts: ["Bar", "Dot"]
            }
        }

        DragArea {
            text: "Pattern"
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Audio Level", "Smooth"]
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Eos Channel", "Basic Transform"]
            }
        }
    }
}


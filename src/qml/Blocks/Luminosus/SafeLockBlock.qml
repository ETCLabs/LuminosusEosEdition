import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    height: 30*dp
    width: 120*dp

    StretchColumn {
        anchors.fill: parent

        DragArea {
            text: "Restricted"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}

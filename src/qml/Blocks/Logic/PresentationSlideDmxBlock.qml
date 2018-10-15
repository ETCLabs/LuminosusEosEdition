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

        StretchText {
            hAlign: Text.AlignHCenter
            text: "DMX -> Slide"
        }

        DragArea {
            text: "PowerPoint"

            InputNode {
                node: block.node("inputNode")
                suggestions: ["sACN In"]
            }
        }
    }  // end main column
}


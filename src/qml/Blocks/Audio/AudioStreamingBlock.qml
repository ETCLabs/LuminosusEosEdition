import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 130*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        TextInput {
            id: textInput
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 4*dp
            anchors.rightMargin: 4*dp
            height: 30*dp
            hintText: "URL"
            text: block.attr("url").val
            onDisplayTextChanged: {
                if (displayText !== block.attr("url").val) {
                    block.attr("url").val = displayText
                }
            }
            clip: true
        }
        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNode {
                node: block.node("inputNode")
            }
            ButtonSideLine {
                text: "Play"
                onPress: block.play()
            }
        }
        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNode {
                node: block.node("stop")
            }
            ButtonSideLine {
                text: "Stop"
                onPress: block.stop()
            }
        }
        DragArea {
            text: "Streaming"
        }
    }  // end main column
}

import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: Math.max(textInput.implicitWidth + 30*dp, 100*dp)
    height: 60*dp
    onWidthChanged: block.positionChanged()

    StretchColumn {
        anchors.fill: parent

        TextInput {
            id: textInput
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 4*dp
            anchors.rightMargin: 4*dp
            height: 30*dp
            hintText: "Command"
            text: block.attr("command").val
            onDisplayTextChanged: {
                if (displayText !== block.attr("command").val) {
                    block.attr("command").val = displayText
                }
            }
        }

        DragArea {
            text: "Command"

            InputNode {
                node: block.node("inputNode")
            }
        }
    }
}


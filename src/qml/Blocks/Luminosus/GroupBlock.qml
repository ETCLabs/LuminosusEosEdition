import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 180*dp
    height: 150*dp

    StretchColumn {
        anchors.fill: parent

        TextInput {
            text: block.attr("label").val
            inputMethodHints: Qt.ImhPreferLatin
            onDisplayTextChanged: {
                if (block.attr("label").val !== displayText) {
                    block.attr("label").val = displayText
                }
            }
            hintText: "Group Name"
            horizontalAlignment: Text.AlignHCenter
        }

        ButtonSideLine {
            text: "Open"
            onPress: block.show()
        }

        StretchText {
            text: "Move to Group:"
            color: "#bbb"
            hAlign: Text.AlignHCenter
        }

        BlockRow {
            ButtonBottomLine {
                text: "Focused Block"
                allUpperCase: false
                onPress: block.moveFocusedBlockToGroup()
            }
            ButtonBottomLine {
                width: 40*dp
                implicitWidth: 0
                text: "All"
                allUpperCase: false
                onPress: block.moveAllBlocksToGroup()
            }
        }

        DragArea {
            text: "Group"
        }
    }  // end main Column
}


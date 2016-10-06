import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: Math.max(codeInput.implicitWidth + 30*dp, 100*dp)
    height: 150*dp

    onWidthChanged: block.positionChanged()

	StretchColumn {
        anchors.fill: parent

		TextInput {
			id: codeInput
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 4*dp
			anchors.rightMargin: 4*dp
			height: 30*dp
            hintText: "v = "
			text: block.code
			onTextChanged: {
				if (text !== block.code) {
					block.code = text
				}
			}
			color: block.codeIsValid ? "#fff" : "#faa"
		}
        BlockRow {
            InputNode {
                objectName: "inputX"
            }
            StretchText {
                text: "x = " + block.variableX.toFixed(2)
            }
        }
        BlockRow {
            InputNode {
                objectName: "inputY"
            }
            StretchText {
                text: "y = " + block.variableY.toFixed(2)
            }
        }
        BlockRow {
            InputNode {
                objectName: "inputZ"
            }
            StretchText {
                text: "z = " + block.variableZ.toFixed(2)
            }
        }


        DragArea {
            text: "Script"
            OutputNode {
                objectName: "outputNode"
            }
		}
    }
}


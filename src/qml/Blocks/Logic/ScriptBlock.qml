import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: Math.max(codeInput.implicitWidth + 30*dp, 100*dp)
    height: 120*dp + codeInput.height

    onWidthChanged: block.positionChanged()

	StretchColumn {
        anchors.fill: parent

        TextEdit {
			id: codeInput
            objectName: "codeInput"
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 4*dp
			anchors.rightMargin: 4*dp
            height: Math.max(30*dp, implicitHeight)
            //hintText: "v = "
			text: block.code
			onTextChanged: {
				if (text !== block.code) {
					block.code = text
				}
			}
            color: block.codeIsValid ? "#fff" : "#faa"
            font.family: "Quicksand"
            font.pixelSize: 18*dp
            selectByMouse: true

            Text {
                height: parent.height
                text: "v = ?"
                visible: parent.text === ""
                color: "#aaa"
                font.pixelSize: 18*dp
                font.family: "Quicksand"
                verticalAlignment:  Text.AlignVCenter
            }
		}
        BlockRow {
            InputNode {
                node: block.node("inputX")
            }
            StretchText {
                text: "x = " + block.variableX.toFixed(2)
            }
        }
        BlockRow {
            InputNode {
                node: block.node("inputY")
            }
            StretchText {
                text: "y = " + block.variableY.toFixed(2)
            }
        }
        BlockRow {
            InputNode {
                node: block.node("inputZ")
            }
            StretchText {
                text: "z = " + block.variableZ.toFixed(2)
            }
        }


        DragArea {
            text: "Script"
            OutputNode {
                node: block.node("outputNode")
            }
		}
    }
}


import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: Math.max(codeInput.implicitWidth + 30*dp, 100*dp)
    height: 60*dp
    pressed: dragarea.pressed

	StretchColumn {
        anchors.fill: parent

		TextInput {
			id: codeInput
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 4*dp
			anchors.rightMargin: 4*dp
			height: 30*dp
			text: block.code
			onTextChanged: {
				if (text !== block.code) {
					block.code = text
				}
			}
			color: block.codeIsValid ? "#fff" : "#faa"
		}


		DragArea {
			id: dragarea
			guiBlock: root
			text: "Script"
			OutputNode {
				objectName: "outputNode"
			}
			InputNode {
				objectName: "inputNode"
			}
		}
    }
}


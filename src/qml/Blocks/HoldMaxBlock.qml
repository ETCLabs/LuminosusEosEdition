import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 100*dp
	height: 60*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		NumericInput {
			minimumValue: 0
			maximumValue: 999
			decimals: 2
			suffix: "s"
			value: block.holdTime
			onValueChanged: {
				if (value !== block.holdTime) {
					block.holdTime = value
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Hold Max"
			OutputNode {
				anchors.left: parent.left
				objectName: "outputNode"
			}
			InputNode {
				anchors.right: parent.right
				objectName: "inputNode"
			}
		}
	}
}


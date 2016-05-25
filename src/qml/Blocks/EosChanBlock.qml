import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 70*dp
	height: 60*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		NumericInput {
			centered: true
			minimumValue: 1
			maximumValue: 32768
			value: block.channelNumber
			onValueChanged: {
				if (value !== block.channelNumber) {
					block.channelNumber = value
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Chan"
			InputNode {
				anchors.right: parent.right
				objectName: "inputNode"
			}
		}
	}
}


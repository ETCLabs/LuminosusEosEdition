import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 130*dp
	height: 60*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent
		BlockRow {
			leftMargin: 5*dp
			rightMargin: 5*dp
			NumericInput {
				width: 40*dp
				implicitWidth: 0  // do not stretch
				minimumValue: 1
				maximumValue: 999
				value: block.cueList
				onValueChanged: {
					if (value !== block.cueList) {
						block.cueList = value
					}
				}
			}
			Text {
				width: 20*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "/"
			}
			NumericInput {
				minimumValue: 1
				maximumValue: 9999
				decimals: 2
				trailingZeros: false
				value: block.cueNumber
				onValueChanged: {
					if (value !== block.cueNumber) {
						block.cueNumber = value
					}
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Cue"
			InputNode {
				objectName: "inputNode"
			}
		}
	}  // end main column
}

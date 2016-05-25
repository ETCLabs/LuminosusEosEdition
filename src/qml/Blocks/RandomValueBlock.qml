import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 130*dp
	height: 90*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			Text {
				width: 60*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "Fade:"
			}
			NumericInput {
				minimumValue: 0
				maximumValue: 100
				suffix: "%"
				value: block.fadeRatio * 100
				onValueChanged: {
					if (Math.abs(value - block.fadeRatio * 100) > 1) {
						block.fadeRatio = value / 100
					}
				}
			}
		}

		BlockRow {
			Text {
				width: 60*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "Step:"
			}
			NumericInput {
				minimumValue: 0.05
				maximumValue: 999
				suffix: "s"
				decimals: 1
				value: block.stepTime
				onValueChanged: {
					if (value !== block.stepTime) {
						block.stepTime = value
					}
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Random"
			OutputNode {
				objectName: "outputNode"
			}
		}
	}  // end main column
}


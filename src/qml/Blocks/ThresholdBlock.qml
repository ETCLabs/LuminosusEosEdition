import QtQuick 2.5
import Qt.labs.controls 1.0
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 80*dp
	height: 200*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		Item {
			height: 30*dp
			Text {
				text: "above"
				anchors.centerIn: parent
			}
			OutputNode {
				anchors.left: parent.left
				objectName: "outputNode"
			}
		}

		Slider {
			value: block.threshold
			onValueChanged: {
				if (value !== block.threshold) {
					block.threshold = value
				}
			}
			indicator: block.inputValue
			useIndicator: true
		}

		NumericInput {
			minimumValue: 0
			maximumValue: 100
			suffix: "%"
			value: block.threshold * 100
			onValueChanged: {
				if (Math.abs(value - (block.threshold * 100)) > 1) {
					block.threshold = value / 100
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "below"
			OutputNode {
				anchors.left: parent.left
				objectName: "outputNodeBelow"
			}
			InputNode {
				anchors.right: parent.right
				objectName: "inputNode"
			}
		}
	}
}


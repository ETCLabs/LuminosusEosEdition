import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: Math.max(oscMessageInput.implicitWidth + 30*dp, 100*dp)
	height: 60*dp
	pressed: dragarea.pressed
	onWidthChanged: block.positionChanged()
	settingsComponent: settings

	StretchColumn {
		anchors.fill: parent

		TextInput {
			id: oscMessageInput
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 4*dp
			anchors.rightMargin: 4*dp
			height: 30*dp
			hintText: "path"
			text: block.message
			onTextChanged: {
				if (text !== block.message) {
					block.message = text
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "OSC"
			OutputNode {
				objectName: "outputNode"
			}

			// ------------ Rx Status LED -----------

			Rectangle {
				width: 10*dp
				height: 10*dp
				radius: width / 2
				anchors.verticalCenter: parent.verticalCenter
				anchors.right: parent.right
				anchors.rightMargin: 10*dp
				color: ledTimer.running ? "lightgreen" : "#777"

				Timer {
					// this timer is triggered when the LED should light up
					// it stops running 200ms after that
					id: ledTimer
					repeat: false
					interval: 200
				}

				Connections {
					target: block
					// trigger the timer when a valid message has been received:
					onValidMessageReceived: ledTimer.restart()
				}
			}
		}
	}  // end main Column

	Component {
		id: settings
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp

			StretchText {
				text: "Value Range:"
			}
			BlockRow {
				StretchText {
					text: "From:"
				}
				NumericInput {
					width: 40*dp
					minimumValue: 0
					maximumValue: 999
					decimals: 1
					value: block.minValue
					onValueChanged: {
						if (value !== block.minValue) {
							block.minValue = value
						}
					}
				}
				StretchText {
					text: "To:"
				}
				NumericInput {
					width: 40*dp
					minimumValue: 0.1
					maximumValue: 999
					value: block.maxValue
					decimals: 1
					onValueChanged: {
						if (value !== block.maxValue) {
							block.maxValue = value
						}
					}
				}
			}
		}
	}  // end Settings Component
}


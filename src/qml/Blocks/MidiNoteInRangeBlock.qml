import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 210*dp
	height: 90*dp
	pressed: dragarea.pressed
	settingsComponent: settings

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			Text {
				width: 60*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "From: "
			}
			ComboBox2 {
				values: controller.midi().getToneNames()
				currentIndex: block.tone
				onValueChanged: {
					if (currentIndex !== block.tone) {
						block.tone = currentIndex
					}
				}
			}
			ComboBox2 {
				values: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
				currentIndex: block.octave
				onCurrentIndexChanged: {
					if (currentIndex !== block.octave) {
						block.octave = currentIndex
					}
				}
			}
			Text {
				width: 30*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "="
			}
			NumericInput {
				value: block.key
				minimumValue: 0
				maximumValue: 127
				onValueChanged: {
					if (value !== block.key) {
						block.key = value
					}
				}
			}
		}

		BlockRow {
			Text {
				width: 60*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "To: "
			}
			ComboBox2 {
				values: controller.midi().getToneNames()
				currentIndex: block.tone2
				onValueChanged: {
					if (currentIndex !== block.tone2) {
						block.tone2 = currentIndex
					}
				}
			}
			ComboBox2 {
				values: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
				currentIndex: block.octave2
				onCurrentIndexChanged: {
					if (currentIndex !== block.octave2) {
						block.octave2 = currentIndex
					}
				}
			}
			Text {
				width: 30*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "="
			}
			NumericInput {
				value: block.key2
				minimumValue: 0
				maximumValue: 127
				onValueChanged: {
					if (value !== block.key2) {
						block.key2 = value
					}
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "MIDI Range"
			OutputNode {
				anchors.left: parent.left
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
	}  // end main column



	Component {
		id: settings
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp

			BlockRow {
				Text {
					text: "Use Default:"
					width: parent.width - 30*dp
				}
				CheckBox {
					id: useDefaultCheckbox
					width: 30*dp
					active: block.useDefaultChannel
					onActiveChanged: {
						if (active !== block.useDefaultChannel) {
							block.useDefaultChannel = active
						}
					}
				}
			}
			BlockRow {
				Text {
					text: "Channel:"
					width: parent.width - 40*dp
					enabled: !useDefaultCheckbox.active
				}
				NumericInput {
					width: 40*dp
					minimumValue: 1
					maximumValue: 16
					value: block.channel
					onValueChanged: {
						if (value !== block.channel) {
							block.channel = value
						}
					}
					enabled: !useDefaultCheckbox.active
				}
			}
		}
	}  // end Settings Component
}


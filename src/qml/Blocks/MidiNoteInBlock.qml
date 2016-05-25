import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 150*dp
	height: 60*dp
	pressed: dragarea.pressed
	settingsComponent: settings

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			ComboBox2 {
				id: toneComboBox
				values: controller.midi().getToneNames()
				property bool initialized: false
				Component.onCompleted: {
					currentIndex = block.tone
					initialized = true
				}
				onValueChanged: {
					if (!initialized) return
					if (currentIndex !== block.tone) {
						block.tone = currentIndex
					}
				}
				Connections {
					target: block
					onKeyChanged: toneComboBox.currentIndex = block.tone
				}
			}
			ComboBox2 {
				id: octaveComboBox
				values: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
				property bool initialized: false
				Component.onCompleted: {
					currentIndex = block.octave
					initialized = true
				}
				onCurrentIndexChanged: {
					if (!initialized) return
					if (currentIndex !== block.octave) {
						block.octave = currentIndex
					}
				}
				Connections {
					target: block
					onKeyChanged: octaveComboBox.currentIndex = block.octave
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

		DragArea {
			id: dragarea
			guiBlock: root
			text: "MIDI"
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


import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 130*dp
    height: 60*dp
	settingsComponent: settings

	StretchColumn {
		anchors.fill: parent
		BlockRow {
			Text {
				width: 90*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "Program:"
			}
			NumericInput {
                value: block.program
				minimumValue: 1
				maximumValue: 128
				onValueChanged: {
                    if (value !== block.program) {
                        block.program = value
					}
				}
			}
		}

        DragArea {
			text: "MIDI"
			OutputNode {
				objectName: "outputNode"
			}

			// ------------ Program is Active LED -----------

			Rectangle {
				width: 10*dp
				height: 10*dp
				radius: width / 2
				anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10*dp
				color: block.programIsActive ? "lightgreen" : "#777"
                antialiasing: false
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
                    text: "MIDI Channel:"
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
            BlockRow {
                StretchText {
                    text: "Learn:"
                }
                ButtonBottomLine {
                    text: block.learning ? "Cancel" : "Start"
                    onPress: block.startLearning()
                }
            }
		}
	}  // end Settings Component
}


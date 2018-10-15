import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
    width: 115*dp + Math.min(250*dp, cueLabel.textWidth)
    height: 60*dp
    settingsComponent: settings

    onWidthChanged: block.positionChanged()

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0.8, 0.65, 0.3, 0.3)
        visible: block.cueObject && block.cueObject.isActive
    }

	StretchColumn {
        anchors.fill: parent

		BlockRow {
			leftMargin: 5*dp
			rightMargin: 5*dp

            // Cue Number input:
			NumericInput {
                width: 35*dp
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
                width: 10*dp
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				text: "/"
			}
			NumericInput {
                width: 60*dp
                implicitWidth: 0  // do not stretch
                minimumValue: 0
                maximumValue: 9999.99
				decimals: 2
				trailingZeros: false
				value: block.cueNumber
				onValueChanged: {
					if (value !== block.cueNumber) {
						block.cueNumber = value
					}
				}
			}

            // Cue Label:
            StretchText {
                id: cueLabel
                text: ": " + (block.cueObject ? block.cueObject.label : "Not Found")
            }
		}

        DragArea {
			text: "Cue"
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Button", "OSC In", "MIDI Control In", "MIDI Note In"]
			}
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["MIDI Control Out", "MIDI Note Out"]
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
                    text: "Cue List:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 999
                    value: block.cueList
                    onValueChanged: {
                        if (value !== block.cueList) {
                            block.cueList = value
                        }
                    }
                }
            }
            BlockRow {
                Text {
                    text: "Cue Number:"
                    width: parent.width - 70*dp
                }
                NumericInput {
                    minimumValue: 1
                    maximumValue: 9999.99
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
        }
    }  // end Settings Component
}

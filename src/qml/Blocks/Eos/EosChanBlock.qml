import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
    width: 70*dp
    height: 115*dp
    settingsComponent: settings

	StretchColumn {
		anchors.fill: parent

        // ------------- Tombstone --------------

        Item {
            height: 85*dp

            // Tombstone Background Image
            Image {
                width: 50*dp
                height: width * (implicitHeight / implicitWidth)
                anchors.centerIn: parent
                source: "qrc:/images/tombstone.png"
            }

            // Chan number at the top
            NumericInput {
                height: 30*dp
                width: 50*dp
                x: 10*dp
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

            // Chan Value in the middle

            Text {
                anchors.centerIn: parent
                text: block.valueText
            }
        }



        DragArea {
			text: "Chan"

            InputNode {
                node: block.node("inputNode")
                suggestions: ["Eos Channel", "Vertical Patch"]
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Eos Channel"]
            }
		}
    }  // end main Column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Channel Number:"
                    width: parent.width - 40*dp
                }
                NumericInput {
                    width: 40*dp
                    minimumValue: 1
                    maximumValue: 32768
                    value: block.channelNumber
                    onValueChanged: {
                        if (value !== block.channelNumber) {
                            block.channelNumber = value
                        }
                    }
                }
            }
        }
    }  // end Settings Component
}


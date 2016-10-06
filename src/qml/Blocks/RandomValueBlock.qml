import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
    width: 150*dp
    height: 150*dp

	StretchColumn {
		anchors.fill: parent

        BlockRow {
            InputNode {
                objectName: "inputNode"
            }
            Text {
                width: 60*dp
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
        BlockRow {
            leftMargin: 8*dp
            Text {
                width: 90*dp
                text: "Step +/-:"
            }
            NumericInput {
                minimumValue: 0
                maximumValue: 100
                suffix: "%"
                value: block.timeVariation * 100
                onValueChanged: {
                    if (Math.abs(value - block.timeVariation * 100) > 1) {
                        block.timeVariation = value / 100
                    }
                }
            }
        }
        BlockRow {
            leftMargin: 8*dp
			Text {
                width: 90*dp
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
        ButtonBottomLine {
            text: "Sync"
            onPress: block.sync()
        }

        DragArea {
			text: "Random"
			OutputNode {
				objectName: "outputNode"
			}
		}
	}  // end main column
}


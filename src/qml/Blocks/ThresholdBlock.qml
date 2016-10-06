import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 80*dp
    height: 200*dp

	StretchColumn {
		anchors.fill: parent

		Item {
			height: 30*dp
            OutputNode {
                objectName: "outputNode"
            }
			Text {
				text: "above"
				anchors.centerIn: parent
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
            text: "below"
            InputNode {
				objectName: "inputNode"
			}
            OutputNode {
                objectName: "outputNodeBelow"
            }
		}
	}
}


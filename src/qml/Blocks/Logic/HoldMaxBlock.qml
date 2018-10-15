import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: 100*dp
    height: 60*dp

	StretchColumn {
		anchors.fill: parent

		NumericInput {
			minimumValue: 0
			maximumValue: 999
			decimals: 2
			suffix: "s"
			value: block.holdTime
			onValueChanged: {
				if (value !== block.holdTime) {
					block.holdTime = value
				}
			}
		}

        DragArea {
			text: "Hold Max"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
			}
		}
	}
}


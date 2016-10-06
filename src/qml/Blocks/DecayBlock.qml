import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 90*dp
    height: 60*dp

	StretchColumn {
		anchors.fill: parent

		NumericInput {
			minimumValue: 0
			maximumValue: 999
			decimals: 2
			suffix: "s"
			value: block.decay
			onValueChanged: {
				if (value !== block.decay) {
					block.decay = value
				}
			}
		}

        DragArea {
            text: "Decay"
            InputNode {
				objectName: "inputNode"
			}
            OutputNode {
                objectName: "outputNode"
            }
		}
	}
}


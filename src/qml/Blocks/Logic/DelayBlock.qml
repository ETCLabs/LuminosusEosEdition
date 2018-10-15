import QtQuick 2.5
import CustomElements 1.0

import "../../CustomBasics"
import "../../CustomControls"


BlockBase {
	id: root
	width: 120*dp
    height: 90*dp

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			leftMargin: 3*dp
			Text {
				width: 50*dp
				verticalAlignment: Text.AlignVCenter
				text: "On:"
			}
			NumericInput {
				implicitWidth: -1
				value: block.onDelay
				minimumValue: 0
				maximumValue: 999
				decimals: 2
				suffix: "s"
				onValueChanged: {
					if (value !== block.onDelay) {
						block.onDelay = value
					}
				}
			}
		}

		BlockRow {
			leftMargin: 3*dp
			Text {
				width: 50*dp
				verticalAlignment: Text.AlignVCenter
				text: "Off:"
			}
			NumericInput {
				implicitWidth: -1
				value: block.offDelay
				minimumValue: 0
				maximumValue: 999
				decimals: 2
				suffix: "s"
				onValueChanged: {
					if (value !== block.offDelay) {
						block.offDelay = value
					}
				}
			}
		}

        DragArea {
            text: "Delay"
            InputNode {
                node: block.node("inputNode")
			}
            OutputNode {
                node: block.node("outputNode")
            }
		}
	}
}


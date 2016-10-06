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

		BlockRow {
			leftMargin: 3*dp
			StretchText {
                text: (block.value <= 1) ? (block.value * 100).toFixed() + "%" : block.value.toFixed(1)
				hAlign: Text.AlignHCenter
			}
		}

        DragArea {
            text: "Preview"
			InputNode {
				objectName: "inputNode"
            }
		}
	}
}


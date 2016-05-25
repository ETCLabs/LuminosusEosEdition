import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 110*dp
	height: 60*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		Text {
			height: 30*dp
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			text: block.bpm.toFixed(1) + " BPM"
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "BPM"
			OutputNode {
				anchors.left: parent.left
				objectName: "outputNode"
			}
			InputNode {
				anchors.right: parent.right
				objectName: "inputNode"
			}
		}
	}
}


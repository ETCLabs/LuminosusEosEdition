import QtQuick 2.5
import CustomElements 1.0

import "../CustomBasics"
import "../CustomControls"


BlockBase {
	id: root
	width: 180*dp
	height: 90*dp
	pressed: dragarea.pressed

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			ButtonSideLine {
				text: "Add All Blocks"
				onPress: block.addAllBlockTypes()
			}
		}

		BlockRow {
			ButtonSideLine {
				text: "Delete All Blocks"
				onReleased: block.deleteAllBlocks()
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root
			text: "Debug"
		}
	}
}


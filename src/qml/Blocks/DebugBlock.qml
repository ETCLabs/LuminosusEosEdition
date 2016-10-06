import QtQuick 2.5
import CustomElements 1.0

import "../CustomBasics"
import "../CustomControls"


BlockBase {
	id: root
	width: 180*dp
    height: 150*dp

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
                onTouchUp: block.deleteAllBlocks()
			}
		}

        BlockRow {
            leftMargin: 8*dp
            rightMargin: 8*dp
            StretchText {
                text: "Types Available:"
            }
            StretchText {
                implicitWidth: 0  // do not stretch
                width: 30*dp
                text: controller.blockManager().blockList().getAllBlockTypes().length
                hAlign: Text.AlignRight
            }
        }

        BlockRow {
            leftMargin: 8*dp
            rightMargin: 8*dp
            StretchText {
                text: "Instances:"
            }
            StretchText {
                implicitWidth: 0  // do not stretch
                width: 30*dp
                text: controller.blockManager().blockInstanceCount
                hAlign: Text.AlignRight
            }
        }

        DragArea {
			text: "Debug"
		}
	}
}


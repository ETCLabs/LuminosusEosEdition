import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 60*dp
    pressed: dragarea.pressed

	StretchColumn {
        anchors.fill: parent

        Switch {
            objectName: "switch"
			uid: block.getUid() + "switch"
			onActiveChanged: root.block.value = (active ? 1 : 0)
        }

		DragArea {
			id: dragarea
			guiBlock: root

			OutputNode {
				objectName: "outputNode"
			}
		}
    }
}


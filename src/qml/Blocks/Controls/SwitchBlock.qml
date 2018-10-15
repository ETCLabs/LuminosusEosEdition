import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 60*dp

	StretchColumn {
        anchors.fill: parent

        Switch {
            objectName: "switch"
			mappingID: block.getUid() + "switch"
            active: block.value === 1
            onActiveChanged: {
                if (active !== (block.value === 1)) {
                    block.value = active ? 1 : 0
                }
            }
        }

        DragArea {
            OutputNode {
                node: block.node("outputNode")
			}
		}
    }
}


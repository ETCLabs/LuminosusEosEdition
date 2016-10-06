import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 45*dp
    height: 200*dp

	StretchColumn {
        anchors.fill: parent

        HeightResizeArea {
            target: root
            minSize: 120*dp
            maxSize: 600*dp
        }

        Slider {
            id: slider
            objectName: "slider"
			uid: block.getUid() + "slider"
			value: block.value
			onValueChanged: {
				if (value !== block.value) {
					block.value = value;
				}
			}
        }

        DragArea {
			OutputNode {
				objectName: "outputNode"
			}
		}
    }
}


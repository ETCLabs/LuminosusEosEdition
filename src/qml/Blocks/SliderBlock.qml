import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 45*dp
    height: 200*dp
    pressed: dragarea.pressed

	StretchColumn {
        anchors.fill: parent

		TouchArea {
			height: 20*dp

			property int minSize: 120*dp
			property int maxSize: 600*dp
			property int initialHeight: 0
			property int initialBottom: 0
			property int initialTouchY: 0

			onTouchDown: {
				initialHeight = root.height
				initialBottom = root.y + root.height
				initialTouchY = touch.sceneY
			}

			onTouchMove: {
				root.height = Math.max(minSize, Math.min(initialTouchY - touch.sceneY + initialHeight, maxSize))
				root.y = initialBottom - root.height
				root.block.positionChangedExternal()
			}

			Rectangle {
				height: 1*dp
				width: 20*dp
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				anchors.verticalCenterOffset: -3*dp
				color: "#888"
			}

			Rectangle {
				height: 1*dp
				width: 20*dp
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter: parent.verticalCenter
				anchors.verticalCenterOffset: 3*dp
				color: "#888"
			}
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
			id: dragarea
			guiBlock: root

			OutputNode {
				objectName: "outputNode"
			}
		}
    }
}


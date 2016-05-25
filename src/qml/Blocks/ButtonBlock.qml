import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
	width: 70*dp
	// block is 8*dp smaller that button to end before the rounding of the edges starts
	height: width + 30*dp - 8*dp
    pressed: dragarea.pressed
	settingsComponent: settings

	StretchColumn {
		y: -8*dp
		height: parent.height + 8*dp
		width: parent.width

		PushButton {
			uid: block.getUid() + "button"
			toggle: root.block.toggleMode
			text: root.block.labelText
			onActiveChanged: root.block.value = (active ? 1 : 0)
		}

		DragArea {
            id: dragarea
            guiBlock: root

			OutputNode {
                objectName: "outputNode"
            }

			TouchArea {
				width: 20*dp
				height: 30*dp
				anchors.right: parent.right

				property int minSize: 50*dp
				property int maxSize: 200*dp
				property int initialWidth: 0
				property int initialBottom: 0
				property int initialTouchX: 0

				onTouchDown: {
					initialWidth = root.width
					initialBottom = root.y + root.height
					initialTouchX = touch.sceneX
				}

				onTouchMove: {
					root.width = Math.max(minSize, Math.min(touch.sceneX - initialTouchX + initialWidth, maxSize))
					root.y = initialBottom - root.height
					root.block.positionChangedExternal()
				}

				Rectangle {
					width: 1*dp
					height: 20*dp
					anchors.verticalCenter: parent.verticalCenter
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.horizontalCenterOffset: -3*dp
					color: "#888"
				}

				Rectangle {
					width: 1*dp
					height: 20*dp
					anchors.verticalCenter: parent.verticalCenter
					anchors.horizontalCenter: parent.horizontalCenter
					anchors.horizontalCenterOffset: 3*dp
					color: "#888"
				}
			}
        }
    }

	// -------------------------- Settings ----------------------------

	Component {
		id: settings
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp

			BlockRow {
				Text {
					text: "Toggle Mode:"
					width: parent.width - 30*dp
				}
				CheckBox {
					id: toggleCheckbox
					width: 30*dp
					active: block.toggleMode
					onActiveChanged: {
						if (active !== block.toggleMode) {
							block.toggleMode = active
						}
					}
				}
			}
			BlockRow {
				StretchText {
					text: "Label:"
				}
				TextInput {
					width: parent.width * 0.6
					text: block.labelText
					onTextChanged: {
						if (text !== block.labelText) {
							block.labelText = text
						}
					}
				}
			}
		}
	}  // end Settings Component
}


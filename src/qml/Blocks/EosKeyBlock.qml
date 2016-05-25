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
			objectName: "button"
			uid: block.getUid() + "button"
			text: root.block.labelText
			onActiveChanged: {
				if (active) {
					root.block.onButtonPress();
				} else {
					root.block.onButtonRelease();
				}
			}
		}

		DragArea {
			id: dragarea
			guiBlock: root

			InputNode {
				objectName: "inputNode"
			}

			TouchArea {
				width: 20*dp
				height: 30*dp
				anchors.left: parent.left

				property int minSize: 60*dp
				property int maxSize: 200*dp
				property int initialWidth: 0
				property int initialBottom: 0
				property int initialRight: 0
				property int initialTouchX: 0

				onTouchDown: {
					initialWidth = root.width
					initialBottom = root.y + root.height
					initialRight = root.x + root.width
					initialTouchX = touch.sceneX
				}

				onTouchMove: {
					root.width = Math.max(minSize, Math.min(initialTouchX - touch.sceneX + initialWidth, maxSize))
					root.y = initialBottom - root.height
					root.x = initialRight - root.width
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
					text: "Key:"
					width: parent.width / 2
					verticalAlignment: Text.AlignVCenter
				}
				ComboBox2 {
					id: keyNameComboBox
					width: parent.width / 2
					values: root.block.getKeyNames()
					texts: root.block.getLabelTexts()
					property bool initialized: false
					Component.onCompleted: {
						setValue(root.block.keyName)
						initialized = true
					}
					onCurrentIndexChanged: {
						if (!initialized) return
						if (getValue() !== block.keyName) {
							block.keyName = getValue()
						}
					}
					Connections {
						target: block
						onKeyNameChanged: keyNameComboBox.setValue(block.keyName)
					}
				}
			}
		}
	}  // end Settings Component
}


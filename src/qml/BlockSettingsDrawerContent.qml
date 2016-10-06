import QtQuick 2.5

import "CustomControls"
import "CustomBasics"

VerticalScrollView {
	contentItem: root
	Column {
		id: root
		width: parent.width
		height: implicitHeight
		Text {
			width: parent.width
            height: 30*dp
			text: "Block Settings"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignBottom
			font.family: "Quicksand"
			font.pixelSize: 22*dp
        }
        Text {
            id: blockTypeLabel
            width: parent.width
            height: 30*dp
            text: "Block Type"
            color: "#aaa"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
            font.family: "Quicksand"
            font.pixelSize: 18*dp
        }
		Loader {
			height: 40*dp  // inital height
			anchors.left: parent.left
			anchors.right: parent.right
			id: blockSettings
			Component.onCompleted: updateComponent()
			onVisibleChanged: if (visible) updateComponent()

			function updateComponent() {
				var focusedBlock = controller.blockManager().getFocusedBlock()
				if (focusedBlock) {
					blockSettings.sourceComponent = focusedBlock.getSettingsComponent()
                    blockTypeLabel.text = focusedBlock.getBlockName()
				} else {
					blockSettings.sourceComponent = noFocusedBlockDummy;
                    blockTypeLabel.text = ""
				}
				//gc()
			}

			Connections {
				target: controller.blockManager()
				onFocusChanged: if (root.visible) blockSettings.updateComponent()
			}

			onItemChanged: {
				height = Qt.binding(function() { return Math.max(40*dp, item.implicitHeight) } )
			}
			Behavior on height {
				NumberAnimation { duration: 100 }
			}

			Component {
				id: noFocusedBlockDummy
				Column {
                    width: parent ? parent.width : 0
					height: 60*dp
					Spacer {
						height: 20*dp
					}
					Text {
						width: parent.width
						text: "There is no focused block."
						color: "#fff"
						horizontalAlignment: Text.AlignHCenter
						font.family: "Quicksand"
						font.pixelSize: 16*dp
					}
				}
			}
		}

		HorizontalDivider {  // ------------------------------------------------------
		}

		Text {
			width: parent.width
			height: 40*dp
			text: "Help"
			color: "#fff"
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignTop
			font.family: "Quicksand"
			font.pixelSize: 22*dp
		}
		Text {
			id: helpText
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 8*dp
			anchors.rightMargin: 2*dp
			height: implicitHeight
			text: "No help available."
			color: "#bbb"
			horizontalAlignment: Text.AlignLeft
			font.family: "Tahoma" //"Quicksand"
			font.pixelSize: 13*dp
			font.letterSpacing: 1*dp
			font.weight: Font.Normal
			wrapMode: Text.Wrap
            textFormat: Text.AutoText
            // HTML Links:
            linkColor: "lightblue"
            onLinkActivated: Qt.openUrlExternally(link)

			Component.onCompleted: updateHelp()
			onVisibleChanged: if (visible) updateHelp()

			function updateHelp() {
				var focusedBlock = controller.blockManager().getFocusedBlock()
				if (focusedBlock) {
					text = focusedBlock.getHelpText()
					if (!text) {
						text = "No help available."
					}
				} else {
					text = "No help available."
				}
			}

			Connections {
				target: controller.blockManager()
				onFocusChanged: if (root.visible) helpText.updateHelp()
			}
		}  // end helpText
	}  // end main Column
}  // end ScrollView


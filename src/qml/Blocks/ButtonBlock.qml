import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
	width: 70*dp
	// block is 8*dp smaller that button to end before the rounding of the edges starts
    height: width + 30*dp - 8*dp
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
            WidthResizeAreaLeft {
                target: root
                minSize: 60*dp
                maxSize: 200*dp
            }

			OutputNode {
                objectName: "outputNode"
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


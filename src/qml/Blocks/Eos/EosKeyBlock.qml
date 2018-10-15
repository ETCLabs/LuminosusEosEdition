import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

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
			objectName: "button"
			mappingID: block.getUid() + "button"
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
            InputNode {
                node: block.node("inputNode")
			}

            WidthAndHeightResizeArea {
                target: root
                minSize: 50*dp
                maxSize: 200*dp
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
            BlockRow {
                visible: block.keyName === "custom"
                StretchText {
                    text: "Label:"
                }
                TextInput {
                    id: labelInput
                    width: parent.width * 0.6
                    text: block.customLabel
                    onTextChanged: {
                        if (text !== block.customLabel) {
                            block.customLabel = text
                        }
                    }
                    Keys.onTabPressed: keyNameInput.forceActiveFocus()
                }
            }
            BlockRow {
                visible: block.keyName === "custom"
                StretchText {
                    text: "OSC Name:"
                }
                TextInput {
                    id: keyNameInput
                    width: parent.width * 0.6
                    text: block.customKeyName
                    onTextChanged: {
                        if (text !== block.customKeyName) {
                            block.customKeyName = text
                        }
                    }
                    Keys.onBacktabPressed: labelInput.forceActiveFocus()
                }
            }

            BlockRow {
                Text {
                    text: "Run in background:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("runInBackground")
                }
            }
		}
	}  // end Settings Component
}


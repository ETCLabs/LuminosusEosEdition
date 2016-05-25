import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 60*dp
    height: 60*dp
    pressed: dragarea.pressed
    settingsComponent: settings

	StretchColumn {
        anchors.fill: parent

        BlockRow {
            OutputNode {
                objectName: "outputNode"
                width: 15*dp

            }
            Item {
				width: 30*dp
                Image {
                    source: "qrc:/images/light_glow.png"
                    width: 90*dp
                    height: 90*dp
                    anchors.centerIn: parent
                    opacity: Math.max(0, block.value*1.2 - 0.2)
                }
                Rectangle {
                    width: 30*dp
                    height: 30*dp
                    radius: 17*dp
                    color: Qt.rgba(block.value, block.value, block.value, 1)
                }
                Image {
                    source: "qrc:/images/light_glow_foreground.png"
                    width: 30*dp
                    height: 30*dp
                    opacity: block.value
                }
                TouchArea {
                    anchors.fill: parent
                    property real initialValue: 0.0
                    property real touchStartY: 0.0
                    onTouchDown: {
                        initialValue = block.value
                        touchStartY = touch.y
                    }

                    onTouchMove: {
                        var value = initialValue + (touchStartY - touch.y) / (120*dp)
                        block.value = Math.max(0, Math.min(value, 1))
                    }
                }
            }

            InputNode {
                objectName: "inputNode"
                width: 15*dp

            }
        }

        DragArea {
            id: dragarea
			guiBlock: parent.parent
            text: root.block.getGuiName()
            Connections {
                target: block
                onNameChanged: {
                    dragarea.text = root.block.getGuiName()
                }
                onChannelChanged: {
                    dragarea.text = root.block.getGuiName()
                }
            }
        }
    }

    Component {
		id: settings
		StretchColumn {
			leftMargin: 15*dp
			rightMargin: 15*dp
			defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Name"
                    width: parent.width / 2
                }
                TextInput {
                    text: block.getName()
                    width: parent.width / 2
                    inputMethodHints: Qt.ImhPreferLatin
                    onTextChanged: {
                        block.setName(text)
                    }
                }
            }
            BlockRow {
                Text {
                    text: "Channel"
                    width: parent.width / 2
                }
                TextInput {
                    width: parent.width / 2
                    text: block.channel
                    hintText: "1"
                    inputMethodHints: Qt.ImhDigitsOnly
                    onTextChanged: {
                        if (!text) return
						if (parseInt(text) !== block.channel) {
							block.channel = Math.max(1, Math.min(8192, parseInt(text)))
						}
                    }
                }
            }
        }
    }
}


import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 60*dp
    height: 60*dp
    settingsComponent: settings

	StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                objectName: "inputNode"
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
                CustomTouchArea {
                    anchors.fill: parent
                    property real initialValue: 0.0
                    property real touchStartY: 0.0
                    onTouchDown: {
                        initialValue = block.value
                    }

                    onTouchMove: {
                        var value = initialValue + (touch.originY - touch.y) / (120*dp)
                        block.value = Math.max(0, Math.min(value, 1))
                    }
                }
            }
            OutputNode {
                objectName: "outputNode"
                width: 15*dp

            }
        }

        DragArea {
            id: dragarea
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
                StretchText {
                    text: "Channel:"
                }
                NumericInput {
                    width: 60*dp
                    implicitWidth: 0  // do not stretch
                    minimumValue: 1
                    maximumValue: 8192
                    value: block.channel
                    onValueChanged: {
                        if (value !== block.channel) {
                            block.channel = value
                        }
                    }
                }
            }
        }
    }
}


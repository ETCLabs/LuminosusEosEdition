import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 140*dp
    height: (block.attr("numChannels").val + 2) * 30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 10*dp
            rightMargin: 5*dp
            StretchText {
                text: "Start Ch.:"
            }
            AttributeNumericInput {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("channel")
                centered: true
            }
        }

        BlockRow {
            leftMargin: 10*dp
            StretchText {
                text: "Channel"
            }
            Text {
                width: 30*dp
                horizontalAlignment: Text.AlignHCenter
                text: (block.attr("channel").val).toFixed(0)
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["DMX Value Range", "Threshold"]
            }
        }

        Repeater {
            id: channelRepeater
            model: block.attr("actualChannelCount").val - 1

            BlockRow {
                leftMargin: 10*dp
                StretchText {
                    text: "Channel"
                }
                Text {
                    width: 30*dp
                    horizontalAlignment: Text.AlignHCenter
                    text: (block.attr("channel").val  + modelData + 1).toFixed(0)
                }
                OutputNode {
                    node: block.node(modelData.toFixed(0))
                    suggestions: ["DMX Value Range", "Threshold"]
                }
            }
        }

        DragArea {
            ButtonSideLine {
                width: 30*dp
                text: "+"
                onPress: block.attr("numChannels").val = block.attr("numChannels").val + 1
            }
            ButtonSideLine {
                width: 30*dp
                x: width
                text: "-"
                onPress: block.attr("numChannels").val = block.attr("numChannels").val -1
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
                    text: "Universe:"
                }
                AttributeNumericInput {
                    width:  55*dp
                    implicitWidth: 0
                    attr: block.attr("universe")
                }
            }
        }
    }  // end Settings Component
}


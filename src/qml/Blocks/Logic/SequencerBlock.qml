import QtQuick 2.5
import CustomElements 1.0
import CustomStyle 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: mainCol.implicitHeight

    StretchColumn {
        id: mainCol
        anchors.fill: parent
        defaultSize: 30*dp

        BlockRow {
            height: 30*dp
            ButtonSideLine {
                text: "+"
                onClick: {
                    block.addNode()
                    root.y -= 30*dp
                    block.positionChangedExternal()
                }
            }
            ButtonSideLine {
                text: "-"
                onClick: {
                    if (block.dynamicNodeCount > 2) {
                        root.y += 30*dp
                        block.positionChangedExternal()
                    }
                    block.removeDynamicNode()
                }
            }
        }

        Repeater {
            model: block.dynamicNodeCount

            BlockRow {
                InputNode {
                    node: block.node(block.dynamicNodeNames[modelData])
                    suggestions: ["Pixel Control"]
                }
                Item {
                    implicitWidth: -1
                    Rectangle {
                        height: 2*dp
                        width: parent.width - 8*dp
                        anchors.centerIn: parent
                        color: Style.primaryActionColor
                        opacity: (modelData === block.stepNumber) ? (1 - block.fadePosition)
                                  : (modelData === (block.stepNumber + 1) % block.dynamicNodeCount) ? block.fadePosition : 0.0
                    }
                }
            }
        }

        ButtonBottomLine {
            text: "Sync"
            onPress: block.sync()
        }

        BlockRow {
            leftMargin: 3*dp
            StretchText {
                text: "Fade:"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("fade")
            }
        }

        BlockRow {
            InputNodeRect {
                node: block.node("time")
                suggestions: ["Beat Detection", "BPM"]
            }

            AttributeNumericInput {
                implicitWidth: -1
                attr: block.attr("time")
                decimals: 2
                suffix: "s"
            }
        }

        DragArea {
            text: "Seq"
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Eos Channel", "Vertical Patch"]
            }
        }
    }
}

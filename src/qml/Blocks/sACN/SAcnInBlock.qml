import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 160*dp
    height: 90*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            leftMargin: 10*dp
            StretchText {
                text: "Universe:"
            }
            AttributeNumericInput {
                width: 55*dp
                implicitWidth: 0
                attr: block.attr("universe")
            }
        }

        BlockRow {
            leftMargin: 10*dp
            StretchText {
                text: "Channel:"
            }
            AttributeNumericInput {
                width: 55*dp
                implicitWidth: 0
                attr: block.attr("channel")
            }
        }

        DragArea {
            text: "sACN In"
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["DMX Value Range", "Threshold"]
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
                    text: "16 bit:"
                }
                AttributeCheckbox {
                    width:  30*dp
                    attr: block.attr("16bit")
                }
            }
        }
    }  // end Settings Component
}


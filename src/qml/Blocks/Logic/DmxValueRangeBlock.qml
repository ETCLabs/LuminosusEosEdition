import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 60*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            AttributeNumericInput {
                centered: true
                attr: block.attr("low")
            }
            Text {
                width: 15*dp
                text: "-"
            }
            AttributeNumericInput {
                centered: true
                attr: block.attr("high")
            }
        }

        DragArea {
            InputNode {
                node: block.node("inputNode")
                suggestions: ["sACN In"]
            }
            OutputNode {
                node: block.node("outputNode")
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
                    text: "On / Off only:"
                }
                AttributeCheckbox {
                    width:  30*dp
                    attr: block.attr("switch")
                }
            }
        }
    }  // end Settings Component
}


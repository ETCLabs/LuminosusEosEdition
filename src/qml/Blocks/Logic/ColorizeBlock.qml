import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 60*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                node: block.node("stepsNode")
                suggestions: ["Pixel Control"]
            }
            StretchText {
                text: "Steps:"
            }
            AttributeNumericInput {
                width: 60*dp
                attr: block.attr("stepCount")
            }
        }
        DragArea {
            text: "Gradient"
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Slider"]
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }  // end main column


    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Smooth:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("smooth")
                }
            }

            BlockRow {
                Text {
                    text: "Horizontal:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("horizontal")
                }
            }
        }
    }  // end Settings Component
}

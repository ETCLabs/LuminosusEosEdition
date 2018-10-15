import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: Math.min(400*dp, 30*dp * block.attr("matrixWidth").val)
    height: Math.min(400*dp, 30*dp * block.attr("matrixHeight").val) + 30*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        ImagePainter {
            width: Math.min(400*dp, 30*dp * block.attr("matrixWidth").val)
            height: Math.min(400*dp, 30*dp * block.attr("matrixHeight").val)
            image: block.matrixImage
            smooth: false
        }

        DragArea {
            text: ""

            InputNode {
                node: block.node("inputNode")
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
                StretchText {
                    text: "Address:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("address")
                }
            }

            BlockRow {
                StretchText {
                    text: "Gamma:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("gamma")
                    decimals: 1
                }
            }

            BlockRow {
                StretchText {
                    text: "Width:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("matrixWidth")
                }
            }

            BlockRow {
                StretchText {
                    text: "Height:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("matrixHeight")
                }
            }
        }
    }
}

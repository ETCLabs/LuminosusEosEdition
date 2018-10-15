import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 70*dp
    height: 370*dp
    settingsComponent: settings

    StretchColumn {
        anchors.fill: parent

        HeightResizeArea {
            height: 20*dp
            target: root
            minSize: 280*dp
            maxSize: 600*dp
        }

        Text {
            height: 30*dp
            horizontalAlignment: Text.AlignHCenter
            text: "Aux"
        }

        AttributeCombobox {
            height: 30*dp
            attr: block.attr("bus")
            values: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
            texts: ["Main", "Bus 1", "Bus 2", "Bus 3", "Bus 4", "Bus 5", "Bus 6", "Bus 7", "Bus 8", "Bus 9", "Bus 10", "Bus 11", "Bus 12", "Bus 13", "Bus 14", "Bus 15", "Bus 16"]
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            rightMargin: 15*dp
            InputNode {
                node: block.node("onNode")
            }
            AttributeCheckbox {
                implicitWidth: -1
                attr: block.attr("on")
            }
        }

        AttributeSlider {
            id: slider
            attr: block.attr("faderPos")
        }

        StretchText {
            implicitHeight: 0
            height: 20*dp
            text: block.decibel.toFixed(1) + " dB"
            hAlign: Text.AlignHCenter
        }

        BlockRow {
            height: 30*dp
            implicitHeight: 0
            InputNode {
                node: block.node("panNode")
            }
            AttributePanSlider {
                attr: block.attr("pan")
            }
        }

        DragArea {
            InputNode {
                node: block.node("inputNode")
            }
        }
    }  // end main Column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                Text {
                    text: "Bus:"
                    width: parent.width - 70*dp
                }
                AttributeCombobox {
                    width: 70*dp
                    attr: block.attr("bus")
                    values: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]
                    texts: ["Main", "Bus 1", "Bus 2", "Bus 3", "Bus 4", "Bus 5", "Bus 6", "Bus 7", "Bus 8", "Bus 9", "Bus 10", "Bus 11", "Bus 12", "Bus 13", "Bus 14", "Bus 15", "Bus 16"]
                }
            }

            BlockRow {
                Text {
                    text: "Allow +10dB:"
                    width: parent.width - 30*dp
                }
                AttributeCheckbox {
                    width: 30*dp
                    attr: block.attr("boost")
                }
            }
        }
    }  // end Settings Component
}

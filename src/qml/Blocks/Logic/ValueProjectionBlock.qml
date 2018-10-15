import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 100*dp
    height: 180*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            height: 150*dp
            AttributeDoubleSlider {
                implicitWidth: -1
                padding: 20*dp
                attr1: block.attr("inMin")
                attr2: block.attr("inMax")
            }
            Item {
                width: 40*dp

                Item {
                    anchors.fill: parent
                    anchors.topMargin: 20*dp
                    anchors.bottomMargin: 20*dp

                    LineItem {
                        anchors.fill: parent
                        x1: 0
                        y1: (1 - block.attr("inMin").val) * height
                        x2: width
                        y2: (1 - block.attr("outMin").val) * height
                        lineWidth: 2*dp
                        color: "#999"
                    }
                    LineItem {
                        anchors.fill: parent
                        x1: 0
                        y1: (1 - block.attr("inMax").val) * height
                        x2: width
                        y2: (1 - block.attr("outMax").val) * height
                        lineWidth: 2*dp
                        color: "#999"
                    }
                }
            }
            AttributeDoubleSlider {
                implicitWidth: -1
                padding: 20*dp
                attr1: block.attr("outMin")
                attr2: block.attr("outMax")
            }
        }

        DragArea {
            text: "->"
            InputNode {
                node: block.node("inputNode")
            }
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}


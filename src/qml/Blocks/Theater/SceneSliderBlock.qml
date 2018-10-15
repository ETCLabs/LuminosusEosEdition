import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 45*dp
    height: 200*dp

    StretchColumn {
        anchors.fill: parent

        HeightResizeArea {
            target: root
            minSize: 120*dp
            maxSize: 600*dp
        }

        AttributeSlider {
            id: slider
            attr: block.attr("benchValue")
            mappingID: block.getUid() + "slider"

            indicator: block.attr("resultValue").val
            useIndicator: true
        }

        DragArea {
            text: "Scn"
            OutputNode {
                node: block.node("outputNode")
            }
        }
    }
}


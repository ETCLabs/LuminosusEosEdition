import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 100*dp
    height: 90*dp

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            StretchText {
                hAlign: Text.AlignHCenter
                text: "Up:"
            }
            AttributeDotSlider {
                implicitWidth: 0  // fixed
                width: 30*dp
                attr: block.attr("heatUpTime")
            }
        }

        BlockRow {
            StretchText {
                hAlign: Text.AlignHCenter
                text: "Down:"
            }
            AttributeDotSlider {
                implicitWidth: 0  // fixed
                width: 30*dp
                attr: block.attr("coolDownTime")
            }
        }

        DragArea {
            InputNode {
                node: block.node("inputNode")
                suggestions: ["Audio Level", "Audio Volume"]
            }
            text: "Smooth"
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Eos Channel", "1D Pattern"]
            }
        }
    }
}


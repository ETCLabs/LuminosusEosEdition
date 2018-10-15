import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
    width: 130*dp
    height: 150*dp

	StretchColumn {
		anchors.fill: parent

        BlockRow {
            InputNodeRect {
                node: block.node("inputNode")
                suggestions: ["Beat Detection", "BPM"]
            }
            Text {
                width: 60*dp
                text: "Step:"
            }
            AttributeNumericInput {
                attr: block.attr("stepTime")
                suffix: "s"
                decimals: 1
            }
        }
        BlockRow {
            leftMargin: 8*dp
            StretchText {
                text: "Step +/-:"
            }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("timeVariation")
            }
        }
        BlockRow {
            leftMargin: 8*dp
            StretchText {
				text: "Fade:"
            }
            AttributeDotSlider {
                width: 30*dp
                attr: block.attr("fadeRatio")
            }
        }
        ButtonBottomLine {
            text: "Sync"
            onPress: block.sync()
            mappingID: block.getUid() + "sync"
        }

        DragArea {
			text: "Random"
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Value Projection"]
			}
		}
	}  // end main column
}


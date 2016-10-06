import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
    width: 120*dp
    height: 90*dp

	StretchColumn {
		anchors.fill: parent

        ButtonBottomLine {
            height: 30*dp
			text: block.bpm.toFixed(1) + " BPM"
            onPress: block.triggerBeat()
		}
        BlockRow {
            ButtonSideLine {
                text: "/2"
                onPress: block.factor = 0.5
                marked: block.factor === 0.5
            }
            ButtonSideLine {
                text: "1x"
                onPress: block.factor = 1
                marked: block.factor === 1
            }
            ButtonSideLine {
                text: "2x"
                onPress: block.factor = 2
                marked: block.factor === 2
            }
            ButtonSideLine {
                text: "4x"
                onPress: block.factor = 4
                marked: block.factor === 4
            }
        }

        DragArea {
			text: "BPM"
            OutputNode {
				objectName: "outputNode"
			}
            InputNode {
				objectName: "inputNode"
			}
		}
	}
}


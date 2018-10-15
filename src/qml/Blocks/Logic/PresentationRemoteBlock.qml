import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: 120*dp
	height: 150*dp

	StretchColumn {
		anchors.fill: parent
		BlockRow {
			InputNode {
				node: block.node("inputNode")
				suggestions: ["OSC In"]
			}
			ButtonBottomLine {
				text: "Next"
				onPress: block.nextSlide()
				mappingID: block.getUid() + "next"
			}
		}
		BlockRow {
			InputNode {
				node: block.node("prev")
				suggestions: ["OSC In"]
			}
			ButtonBottomLine {
				text: "Previous"
				onPress: block.previousSlide()
				mappingID: block.getUid() + "prev"
			}
		}
		BlockRow {
			InputNode {
				node: block.node("white")
				suggestions: ["OSC In"]
			}
			ButtonBottomLine {
				text: "White"
				onPress: block.whiteSlide()
				mappingID: block.getUid() + "white"
			}
		}
		BlockRow {
			InputNode {
				node: block.node("black")
				suggestions: ["OSC In"]
			}
			ButtonBottomLine {
				text: "Black"
				onPress: block.blackSlide()
				mappingID: block.getUid() + "black"
			}
		}

		DragArea {
			text: "PowerPoint"
		}
	}
}


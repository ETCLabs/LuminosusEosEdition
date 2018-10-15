import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: 110*dp
	height: 90*dp

	StretchColumn {
		anchors.fill: parent

		StretchRow {
			height: 30*dp
			implicitHeight: 0
			leftMargin: 10*dp
			StretchText {
				text: "Slide:"
			}
			AttributeNumericInput {
				width: 60*dp
				attr: block.attr("slideNumber")
			}
		}

		ButtonBottomLine {
			height: 30*dp
			text: "Fire"
			onPress: block.goToSlide()
			mappingID: block.getUid() + "fire"
		}

		DragArea {
			text: "PowerPoint"

			InputNode {
				node: block.node("inputNode")
				suggestions: ["Eos Cue", "MIDI Control In", "MIDI Note In", "OSC In"]
			}
		}
	}  // end main column
}


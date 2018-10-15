import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: Math.max(textInput.implicitWidth + 20*dp, 100*dp)
    height: Math.max(textInput.implicitHeight + 38*dp, 60*dp)
	onWidthChanged: block.positionChanged()

	StretchColumn {
		anchors.fill: parent

		BlockRow {
			TextEdit {
				id: textInput
				anchors.fill: parent
				anchors.margins: 4*dp

				color: "#fff"
				font.family: "Quicksand"
				font.pixelSize: 18*dp
				selectByMouse: true

				text: block.text
				onTextChanged: {
					if (text !== block.text) {
						block.text = text
					}
				}
			}
		}

        DragArea {
			text: "Notes"
		}
	}  // end main Column
}


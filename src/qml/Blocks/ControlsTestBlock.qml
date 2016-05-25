import QtQuick 2.5
import CustomElements 1.0

import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
	width: 120*dp
	height: 210*dp
    pressed: dragarea.pressed

	StretchColumn {
        anchors.fill: parent

		BlockRow {
			StretchColumn {
                width: 30*dp
                Slider {
					uid: block.getUid() + "slider"
                }
            }
			StretchColumn {
				implicitWidth: -1
				ComboBox2 {
					height: 30*dp
					values: ["some", "values", "to", "choose"]
				}

                ButtonBottomLine {
					height: 30*dp
                    uid: block.getUid() + "ButtonBottomLine"
					text: "BUTTON"
                }
                ButtonSideLine {
					height: 30*dp
                    uid: block.getUid() + "ButtonSideLine"
					text: "BUTTON"
                }
                Switch {
					height: 30*dp
					uid: block.getUid() + "Switch"
                }
                IconButton {
					height: 30*dp
                    uid: block.getUid() + "IconButton"
					iconName: "plus_icon"
                }
                TextInput {
					height: 30*dp
					hintText: "hint"
                }
            }
		}
		DragArea {
			id: dragarea
			guiBlock: root
		}
    }
}


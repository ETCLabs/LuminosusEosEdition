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

		ButtonBottomLine {
			toggle: true
			height: 30*dp
			text: "Key: " + (active ? "?" : block.keyName)
			allUpperCase: false
			focus: active
			mappingID: block.getUid() + "change"
			Keys.onPressed: {
				if (active) {
					if (event.modifiers !== Qt.KeypadModifier && event.modifiers !== Qt.NoModifier) {
						controller.guiManager().showToast("Modifiers are not supported.")
						return
					}
					block.attr("fromKeyPad").val = event.modifiers & Qt.KeypadModifier
					block.attr("keyName").val = event.text
					block.attr("scanCode").val = event.nativeScanCode
					active = false
				}
			}
		}

		ButtonBottomLine {
			text: "Fire"
			onActiveChanged: active ? block.pressKey() : block.releaseKey()
			mappingID: block.getUid() + "fire"
		}

		DragArea {
			text: "Key"

			InputNode {
				node: block.node("inputNode")
				suggestions: ["Eos Cue", "OSC In"]
			}

			OutputNode {
				node: block.node("outputNode")
				suggestions: ["Key Press"]
			}
		}
	}  // end main column
}


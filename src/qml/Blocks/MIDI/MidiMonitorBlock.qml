import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
	id: root
	width: 450*dp
    height: 300*dp
	onWidthChanged: block.positionChanged()

	StretchColumn {
		anchors.fill: parent

		// ---------------------- Incoming / Outgoing Checkboxes -----------

		BlockRow {
			height: 30*dp
			implicitHeight: 0 // do not stretch
			ButtonBottomLine {
				id: logIncomingCheckbox
				onTouchDown: controller.midi().setLogInput(active)
				text: "Incoming"
				toggle: true
			}
			ButtonBottomLine {
				id: logOutgoingCheckbox
				onTouchDown: controller.midi().setLogOutput(active)
				text: "Outgoing"
				toggle: true
			}
			ButtonBottomLine {
				text: "Clear"
				onPress: controller.midi().clearLog()
			}
            HeightResizeArea {
                width: 30*dp
                target: root
                maxSize: 1000*dp
            }

			Component.onCompleted: {
				// initialize checkbox values:
				logIncomingCheckbox.active = controller.midi().getLogInput()
				logOutgoingCheckbox.active = controller.midi().getLogOutput()
			}
		}

		// --------------- ListView with Log -------------

        CustomTouchArea {
			// this item either displays the listview or a label "No Messages"
			implicitHeight: -1  // stretch height
            onScrollEvent: scrollEventWasAccepted()

			ListView {
				id: logList
				model: controller.midi().getLog()
				clip: true
				verticalLayoutDirection: ListView.BottomToTop
				anchors.fill: parent
				anchors.leftMargin: 5*dp
				visible: logList.count !== 0
				delegate: Text {
                    height: contentHeight + 3*dp
                    anchors.left: parent.left
                    anchors.right: parent.right
					wrapMode: Text.Wrap
					text: modelData
					color: (modelData.indexOf("[In]") !== -1) ? "#5d82dd" : "#aaa"
					font.pixelSize: 14*dp
					font.family: "BPmono"
                }  // end delegate

                // the default cacheBuffer value is greater than zero
                // but a value greater than zeros causes the application to crahs because of
                // the asynchronous creation of delegates and a possible race condition with
                // the debugger
                cacheBuffer: 0

				Connections {
					target: controller.midi()
					onLogChanged: {
						logList.model = controller.midi().getLog()
					}
				}
			}  // end ListView

			Text {
				text: "No Messages"
				visible: logList.count === 0
				anchors.centerIn: parent
			}
		}

        DragArea {
			text: "MIDI Monitor"

            WidthResizeArea {
                target: root
                minSize: 450*dp
                maxSize: 1100*dp
            }
		}
	}
}


import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 450*dp
	height: 300*dp
	pressed: dragarea.pressed
	onWidthChanged: block.positionChanged()

	StretchColumn {
		anchors.fill: parent

		// ---------------------- Incoming / Outgoing Checkboxes -----------

		BlockRow {
			height: 30*dp
			implicitHeight: 0 // do not stretch
			ButtonBottomLine {
				id: logIncomingCheckbox
				onTouchDown: controller.osc().enableLogging(logIncomingCheckbox.active, logOutgoingCheckbox.active)
				text: "Incoming"
				toggle: true
				//textColor: "#88d"
			}
			ButtonBottomLine {
				id: logOutgoingCheckbox
				onTouchDown: {
					controller.osc().enableLogging(logIncomingCheckbox.active, logOutgoingCheckbox.active)
				}
				text: "Outgoing"
				toggle: true
			}
			ButtonBottomLine {
				text: "Clear"
				onPress: controller.osc().clearLog()
			}

			Component.onCompleted: {
				// initialize checkbox values:
				logIncomingCheckbox.active = controller.osc().getLogIncomingIsEnabled()
				logOutgoingCheckbox.active = controller.osc().getLogOutgoingIsEnabled()
			}
		}

		// --------------- ListView with Log -------------

		Item {
			// this item either displays the listview or a label "No Messages"
			implicitHeight: -1  // stretch height

			ListView {
				id: logList
				model: controller.osc().getLog()
				clip: true
				verticalLayoutDirection: ListView.BottomToTop
				anchors.fill: parent
				anchors.leftMargin: 5*dp
				visible: logList.count !== 0
				delegate: Text {
					height: contentHeight + 3*dp
					width: parent.width
					wrapMode: Text.Wrap
					text: modelData
					color: (modelData.indexOf("[In]") !== -1) ? "#5d82dd" : "#aaa"
					font.pixelSize: 14*dp
					font.family: "BPmono"
				}  // end delegate

				onCountChanged: {
					// emit contentYChanged signal, otherwise scrollbar position will be wrong
					contentYChanged()
				}

				Connections {
					target: controller.osc()
					onLogChanged: {
						logList.model = controller.osc().getLog()
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
			id: dragarea
			guiBlock: root
			text: "OSC Monitor"
		}
	}
}


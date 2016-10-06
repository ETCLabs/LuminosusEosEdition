import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
	id: root
	width: 450*dp
    height: 300*dp
	onWidthChanged: block.positionChanged()

	StretchColumn {
		anchors.fill: parent

		// --------------- ListView with Log -------------

		Item {
			// this item either displays the listview or a label "No Messages"
			implicitHeight: -1  // stretch height

			ListView {
				id: logList
				model: controller.logManager().getLog()
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
					color: "#aaa"
					font.pixelSize: 14*dp
					font.family: "BPmono"
                }  // end delegate

                // the default cacheBuffer value is greater than zero
                // but a value greater than zeros causes the application to crahs because of
                // the asynchronous creation of delegates and a possible race condition with
                // the debugger
                cacheBuffer: 0

				Connections {
					target: controller.logManager()
					onLogChanged: {
						logList.model = controller.logManager().getLog()
					}
				}
			}  // end ListView

			Text {
				text: "No Messages"
				visible: logList.count === 0
				anchors.centerIn: parent
			}


            HeightResizeArea {
                width: 30*dp
                height: 30*dp
                target: root
                maxSize: 1000*dp
                anchors.right: parent.right
                anchors.top: parent.top
            }
		}

        DragArea {
			text: "App Log"

            WidthResizeArea {
                target: root
                minSize: 450*dp
                maxSize: 1100*dp
            }
		}
	}
}

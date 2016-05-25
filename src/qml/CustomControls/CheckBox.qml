import QtQuick 2.5
import "../CustomBasics"

TouchArea {
	property bool active: false

	Rectangle {
		width: 20*dp
		height: 20*dp
		anchors.centerIn: parent
		color: "transparent"
		border.width: 1*dp
		border.color: "#555"

		Rectangle {
			color: Qt.rgba(0.3, 0.6, 1, 1)
			anchors.fill: parent
			anchors.margins: active ? 3*dp : 10*dp
			Behavior on anchors.margins {
				NumberAnimation {
					duration: 100
				}
			}
		}
	}

	onTouchDown: {
		active = !active
	}

}

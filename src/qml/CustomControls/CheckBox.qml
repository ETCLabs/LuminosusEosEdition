import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
	property bool active: false
    property bool light: false

	Rectangle {
		width: 20*dp
		height: 20*dp
		anchors.centerIn: parent
		color: "transparent"
		border.width: 1*dp
        border.color: light ? "#888" : "#555"

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

    onClick: {
        controller.setPropertyWithoutChangingBindings(this, "active", !active)
	}

}

import QtQuick 2.5
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"

CustomTouchArea {
	implicitHeight: -1  // use defaultSize

	property var blockInfo
	property bool isCategory: false

	StretchRow {  // row with left margin
		anchors.fill: parent
		leftMargin: 5*dp
		Item {  // circle
			width: 30*dp
			Rectangle {
				width: 8*dp
				height: 8*dp
				radius: height / 2
				anchors.centerIn: parent
				color: "transparent"
				border.width: 1
				border.color: Qt.rgba(0.3, 0.7, 1, 1)
			}
		}

		Text {
			id: label
			width: parent.width - 35*dp
			verticalAlignment: Text.AlignVCenter
			fontSizeMode: Text.Fit
			text: blockInfo.nameInUi
		}
	} // end row with left margin

	Rectangle {  // left line
		height: parent.height
		width: 1*dp
		color: "#555"
	}

	RippleEffect {
		id: rippleEffect
        anchors.fill: parent
    }

    onTouchDown: {
        rippleEffect.touchStart(touch.itemX, touch.itemY)
    }

    onTouchUp: {
		if (!canceled) {
            rippleEffect.touchEnd()
            controller.blockManager().addBlockByNameQml(blockInfo.name)
        } else {
            rippleEffect.touchCancel()
        }
	}
}

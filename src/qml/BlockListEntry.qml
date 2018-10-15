import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0

import "CustomControls"
import "CustomBasics"

CustomTouchArea {
	implicitHeight: -1  // use defaultSize

    property var blockInfo

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
                border.color: Style.primaryActionColor
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

//	RippleEffect { -> disabled due to CPU consumption bug
//		id: rippleEffect
//        anchors.fill: parent
//    }

    TouchFeedbackEffect {
        id: touchFeedbackEffect
    }

    onTouchDown: {
        //rippleEffect.touchStart(touch.itemX, touch.itemY)
    }

    onTouchUp: {
		if (!canceled) {
            controller.playClickSound()
            touchFeedbackEffect.start()
            //rippleEffect.touchEnd()
            controller.blockManager().addBlockByNameQml(blockInfo.name)
        } else {
            //rippleEffect.touchCancel()
        }
	}
}

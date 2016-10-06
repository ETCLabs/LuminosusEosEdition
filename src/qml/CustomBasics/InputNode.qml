import QtQuick 2.5
import CustomElements 1.0

Item {
	property bool focused: false
    property bool htp: false
    width: 15*dp
    height: 30*dp
    anchors.left: parent.left

    signal touched()
    signal longClick()

    Image {
        width: 30*dp
        height: 30*dp
        x: -15*dp
        source: dp <= 1 ? (focused ? "qrc:/images/node_precomposed_red.png" : "qrc:/images/node_precomposed_" + (htp ? "cyan.png" : "blue.png"))
                        : (focused ? "qrc:/images/node_precomposed_red@2x.png" : "qrc:/images/node_precomposed_" + (htp ? "cyan@2x.png" : "blue@2x.png"))
    }

    CustomTouchArea {
        id: touchArea
        width: 30*dp
        height: 30*dp
        x: -15*dp

        clickDurationEnabled: true
        onShortClick: parent.touched()
        onLongClick: {
            parent.longClick()
            if (htp) {
                controller.showToast("HTP Mode")
            } else {
                controller.showToast("LTP Mode")
            }
        }
    }
}


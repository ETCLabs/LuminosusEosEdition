import QtQuick 2.5
import CustomElements 1.0

Item {
	property bool focused: false
    width: 15*dp
    height: 30*dp
    anchors.right: parent.right

    signal touched()

    Image {
        width: 30*dp
        height: 30*dp
        source: dp <= 1 ? (focused ? "qrc:/images/node_precomposed_red.png" : "qrc:/images/node_precomposed_blue.png")
                        : (focused ? "qrc:/images/node_precomposed_red@2x.png" : "qrc:/images/node_precomposed_blue@2x.png")
    }

    CustomTouchArea {
        id: touchArea
        width: 30*dp
        height: 30*dp

        onClick: {
            parent.touched()
        }
    }

    NodeConnectionLines {
        anchors.fill: parent
        objectName: "connectionLines"
        lineWidth: 3*dp
        color: Qt.rgba(0.3, 0.5, 1.0, 1)
        //color: Qt.rgba(0.0, 0.3, 1.0, 0.7)
        z: -1
    }

}

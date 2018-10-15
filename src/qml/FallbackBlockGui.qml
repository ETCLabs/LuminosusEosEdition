import QtQuick 2.5
import "CustomBasics"

BlockBase {
    id: root
    width: 220*dp
    height: 90*dp

    Rectangle {
        anchors.fill: parent
        color: "#a60000"
    }

    Text {
        anchors.fill: parent
        text: "GUI of '" + block.getBlockName() + "'\ncould not be loaded"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}

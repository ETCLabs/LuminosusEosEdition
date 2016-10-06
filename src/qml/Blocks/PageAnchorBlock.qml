import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 120*dp
    height: 30*dp

    visible: !controller.anchorManager().presentationMode

    Rectangle {
        x: -15*dp
        y: -15*dp
        width: 1280*dp
        height: 720*dp
        color: "transparent"
        border.width: 1*dp
        border.color: "#777"
    }

    StretchColumn {
        anchors.fill: parent

        DragArea {
            text: "Page " + (block.anchorIndex + 1)
        }
    }  // end main Column
}


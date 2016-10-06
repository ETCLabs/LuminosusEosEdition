import QtQuick 2.5
import CustomElements 1.0


Item {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    height: 30*dp
    z: -1

    // public
    property string text: ""
    default property alias content: contentItem.data

    // -------------------------- Visuals -----------------------------------

    Rectangle {
        anchors.fill: parent
        color: "#555"
    }

    Rectangle {
        // thin line at the top
        color: block.focused ? Qt.rgba(0.9, 0.8, 0.0, 0.5) : "#666"
        height: 1*dp
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Text {
        color: block.focused ? Qt.rgba(0.9, 0.8, 0.0, 0.5) : "#999"
        font.family: "Quicksand"
        font.weight: Font.Bold
        font.pixelSize: 18*dp
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        fontSizeMode: Text.Fit
        text: block.label || root.text
    }

    Item {
        id: contentItem
        anchors.fill: parent
    }
}


import QtQuick 2.5
import CustomStyle 1.0
import CustomElements 1.0
import ".."

Rectangle {
    id: root
    anchors.left: parent.left
    anchors.right: parent.right
    height: 30*dp
    z: -1
    color: "#555"

    // public
    property string text: ""

    // -------------------------- Visuals -----------------------------------

    Rectangle {
        // thin line at the top
        color: block.focused ? Style.blockFocusColor : "#666"
        height: 1*dp
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Text {
        color: block.focused ? Style.blockFocusColor : "#999"
        font.family: "Quicksand"
        font.weight: Font.Bold
        font.pixelSize: 18*dp
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        fontSizeMode: Text.Fit
        text: block.attr("label").val || root.text
    }
}


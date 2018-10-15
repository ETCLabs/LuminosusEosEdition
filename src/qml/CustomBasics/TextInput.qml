import QtQuick 2.5
import CustomStyle 1.0


TextInput {
	id: root
    property string hintText: ""
	height: 30*dp
    color: "#fff"
    font.family: "Quicksand"
    font.pixelSize: 18*dp
	verticalAlignment: Text.AlignVCenter
    selectByMouse: true

	Rectangle {  // thin blue line under text
        color: Style.primaryActionColor
        height: 1*dp
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: 2*dp
		anchors.rightMargin: 2*dp
		anchors.verticalCenter: root.verticalCenter
		anchors.verticalCenterOffset: 12*dp
        visible: !root.readOnly
    }

    Text {
        height: parent.height
        width: parent.width
        text: hintText
        visible: parent.displayText === ""
        color: "#aaa"
        font.pixelSize: 18*dp
        font.family: "Quicksand"
		verticalAlignment:  Text.AlignVCenter
        horizontalAlignment: root.horizontalAlignment
    }
}


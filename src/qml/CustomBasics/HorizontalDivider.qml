import QtQuick 2.0

Item {
	property alias color: line.color
	height: 30*dp
	anchors.left: parent.left
	anchors.right: parent.right
	Rectangle {
		id: line
		height: 1*dp
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		color: "#aaa"
	}
}

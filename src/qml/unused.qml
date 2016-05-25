SliderBlock {
    plane: plane
    xOnPlane: 100*dp
    yOnPlane: 100*dp
    channel: 305
}

SliderBlock {
    plane: plane
    xOnPlane: 170*dp
    yOnPlane: 120*dp
    channel: 306
}

SliderBlock {
    plane: plane
    xOnPlane: 240*dp
    yOnPlane: 140*dp
    channel: 307
}

BlockBase {
    plane: plane
    width: 90*dp
    height: 60*dp
    xOnPlane: 200*dp
    yOnPlane: 500*dp
    pressed: dragarea2.pressed

    onXChanged: {
        line.updatePoints()
    }
    onYChanged: {
        line.updatePoints()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Switch {
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignCenter
        }

        DragArea {
            id: dragarea2
            target: parent.parent
            Layout.preferredHeight: 30*dp
            Layout.preferredWidth: parent.width

            Rectangle {
                color: Qt.rgba(1, 1, 1, 0.1)
                border.width: 1*dp
                border.color: Qt.rgba(1, 1, 1, 0.4)
                width: 14*dp
                height: 14*dp
                radius: width / 2
                x: parent.width - (width / 2)
                y: (parent.height - height) / 2
            }
        }
    }
}

BlockBase {
    id: node3
    plane: plane
    width: 100*dp
    height: 120*dp
    xOnPlane: 400*dp
    yOnPlane: 300*dp
    pressed: dragarea3.pressed

    onXChanged: {
        line.updatePoints()
    }
    onYChanged: {
        line.updatePoints()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Text {
            text: "Hue: "
            color: "white"
            font.family: "Quicksand"
            font.pixelSize: 16*dp
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
        }
        Text {
            text: "Saturation: "
            color: "white"
            font.family: "Quicksand"
            font.pixelSize: 16*dp
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
        }
        Text {
            text: "Value: "
            color: "white"
            font.family: "Quicksand"
            font.pixelSize: 16*dp
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
        }

        DragArea {
            id: dragarea3
            target: parent.parent
            text: "HSV"
            Layout.preferredHeight: 30*dp
            Layout.preferredWidth: parent.width

            Rectangle {
                color: Qt.rgba(1, 1, 1, 0.1)
                border.width: 1*dp
                border.color: Qt.rgba(1, 1, 1, 0.4)
                width: 14*dp
                height: 14*dp
                radius: width / 2
                x: -(width / 2)
                y: (parent.height - height) / 2
            }
        }
    }

    BezierCurve {
        id: line
        color: Qt.rgba(0.1, 0.3, 0.8, 1)
        lineWidth: 3*dp
        property Item startItem: dragarea2
        property Item endItem: dragarea3
        function updatePoints() {
            startPoint = getStartPointFromItem(startItem)
            endPoint = getEndPointFromItem(endItem)
        }
        function getEndPointFromItem(item) {
            return Qt.point(item.mapToItem(null, 0, 0).x, item.mapToItem(null, 0, 0).y + item.height / 2)
        }
        function getStartPointFromItem(item) {
            return Qt.point(item.mapToItem(null, 0, 0).x + item.width, item.mapToItem(null, 0, 0).y + item.height / 2)
        }
        Component.onCompleted: updatePoints()
    }
}

DimmerBlock {
    plane: plane
    xOnPlane: 550*dp
    yOnPlane: 300*dp
}

ButtonBlock {
    plane: plane
    xOnPlane: 650*dp
    yOnPlane: 360*dp
}

BlockBase {
    plane: plane
    width: 90*dp
    height: 60*dp
    xOnPlane: 500*dp
    yOnPlane: 400*dp
    pressed: dragarea5.pressed

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Button {
            id: halo_button
            text: "Halo"
            toggle: true
            pressed: false
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width
        }

        DragArea {
            id: dragarea5
            target: parent.parent
            Layout.preferredHeight: 30*dp
            Layout.preferredWidth: parent.width
        }
    }
}

BlockBase {
    plane: plane
    width: 120*dp
    height: 150*dp
    xOnPlane: 500*dp
    yOnPlane: 100*dp
    pressed: dragarea6.pressed

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Item {
            id: root
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width

            Rectangle {
                anchors.fill: parent
                anchors.margins: 12*dp
                radius: width / 2
                border.width: 1*dp
                border.color: "yellow"
                color: "transparent"
            }

            Text {
                anchors.centerIn: parent
                text: "35%"
                font.pointSize: 22*dp
                color: "white"
                font.family: "Quicksand"
                font.weight: Font.Light
            }
        }

        DragArea {
            id: dragarea6
            target: parent.parent
            Layout.preferredHeight: 30*dp
            Layout.preferredWidth: parent.width
        }
    }
}

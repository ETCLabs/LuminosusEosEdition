import QtQuick 2.5

import CustomElements 1.0

TouchArea {
    property Item guiBlock
    property alias text: text.text
    property vector2d startPos

	anchors.left: parent.left
	anchors.right: parent.right

	height: 30*dp
    z: -1

	default property alias content: contentItem.data

    Rectangle {
        anchors.fill: parent
        color: "#555"
    }

    Rectangle {
        // thin line at the top
        color: "#666"
		height: 1*dp
		anchors.left: parent.left
		anchors.right: parent.right
    }

    Text {
        id: text
        color: "#999"
        font.family: "Quicksand"
        font.weight: Font.Bold
        font.pixelSize: 18*dp
        anchors.fill: parent
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        fontSizeMode: Text.Fit
    }

	KineticEffect2D {
		id: kineticEffect
        friction: 0.2
        onMoving: {
			guiBlock.x = position.x
			guiBlock.y = position.y
        }
	}

	Connections {
		target: guiBlock.block
		onPositionChangedExternal: {
			kineticEffect.setValue(guiBlock.x, guiBlock.y)
        }
    }

    onTouchDown: {
        startPos = Qt.vector2d(touch.sceneX, touch.sceneY)
		kineticEffect.start(touch.sceneX, touch.sceneY)
    }

	onTouchMove: {
		kineticEffect.update(touch.sceneX, touch.sceneY)
    }

    onTouchUp: {
        var endPos = Qt.vector2d(touch.sceneX, touch.sceneY)
        if (Math.abs(endPos.minus(startPos).length()) < 3*dp) {
            // touch was a tap
            guiBlock.block.focus()
		}
		kineticEffect.stop(touch.sceneX, touch.sceneY)

		// check if it is released in trash area:
		var isInTrash = controller.pointIsInTrashArea(touch.sceneX, touch.sceneY)
		if (isInTrash) {
			controller.blockManager().deleteBlock(guiBlock.block.getUid())
		}
	}

    Item {
        id: contentItem
        anchors.fill: parent
    }
}


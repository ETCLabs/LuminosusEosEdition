import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"

CustomTouchArea {
	id: root
    property int padding: 20*dp
    property real value: 0.0
	property real indicator: 0.0
    property bool useIndicator: false
	implicitHeight: -1

    Rectangle {
        width: 2*dp
        color: "#666"
        height: parent.height - 2 * padding
        anchors.centerIn: parent
    }

    Rectangle {
        width: 2*dp
        color: "#999"
        height: (parent.height - 2 * padding) * value
        anchors.horizontalCenter: parent.horizontalCenter
        y: padding + (parent.height - 2 * padding) * (1 - value)
		visible: !useIndicator
    }

	Rectangle {
		width: 2*dp
		color: Qt.rgba(0.3, 0.6, 1, 0.7)
		height: (parent.height - 2 * padding) * indicator
		anchors.horizontalCenter: parent.horizontalCenter
		y: padding + (parent.height - 2 * padding) * (1 - indicator)
		visible: useIndicator
	}

    Image {
        width: 32*dp
        height: 32*dp
		smooth: false
        source: (width <= 32) ? "qrc:/images/slider_cursor.png" : "qrc:/images/slider_cursor@2x.png"
        x: parent.width / 2 - width / 2
        y: padding + (parent.height - 2*padding) * (1 - value) - height / 2
    }

    KineticEffect {
        id: kineticEffect
        friction: 0.18
        minValue: padding
        maxValue: parent.height - padding
		onMoving: setValueByPosition(position)
    }

    onTouchDown: {
        controller.checkForExternalInputConnection(uid)
        kineticEffect.setValue(touch.itemY)
        kineticEffect.start(touch.itemY)
        setValueByPosition(touch.itemY)
    }

	onTouchMove: {
        kineticEffect.update(Math.max(kineticEffect.minValue, Math.min(touch.itemY, kineticEffect.maxValue)))
    }

	onTouchUp: {
        kineticEffect.stop(Math.max(kineticEffect.minValue, Math.min(touch.itemY, kineticEffect.maxValue)))
	}

	function setValueByPosition(position) {
		var val = 1 - Math.max(0.0, Math.min(1.0, (position - padding) / (height - 2*padding)))
		controller.setPropertyWithoutChangingBindings(root, "value", val)
		// "this" as first parameter does not work, bug in QML?
	}

    property string uid: ""
    property real externalInput: 0
    Component.onCompleted: controller.registerGuiControl(this)
    Component.onDestruction: if (controller) controller.unregisterGuiControl(this)
    onExternalInputChanged: {
		controller.setPropertyWithoutChangingBindings(this, "value", externalInput)
    }
}


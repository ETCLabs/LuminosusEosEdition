import QtQuick 2.5

Item {
	property bool focused: false
    width: 15*dp
    height: 30*dp
	anchors.right: parent.right

    signal touched()

    Image {
        width: 30*dp
        height: 30*dp
		source: "qrc:/images/node_shadow.png"
    }

    Rectangle {
        width: 14*dp
        height: 14*dp
        x: 7*dp
        y: 8*dp
        radius: 7*dp
        color: "#2C89E1"
        border.width: 2*dp
        border.color: "#bbb"
        gradient: focused ? gradient_red : gradient_blue2

        Gradient {
            id: gradient_blue
            GradientStop { position: 0.0; color: "#2C89E1" }
            GradientStop { position: 1.0; color: "#166ED4" }
        }
        Gradient {
            id: gradient_blue2
            GradientStop { position: 0.0; color: "#71ABF0" }
            GradientStop { position: 1.0; color: "#507496" }
        }
        Gradient {
            id: gradient_red
            GradientStop { position: 0.0; color: "red" }
            GradientStop { position: 1.0; color: "darkred" }
        }
    }

    TouchArea {
        id: touchArea
        width: 30*dp
        height: 30*dp

        onTouchDown: {
            parent.touched()
        }
    }
}


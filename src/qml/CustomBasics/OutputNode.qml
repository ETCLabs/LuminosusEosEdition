import QtQuick 2.5
import CustomGeometry 1.0

Item {
	property bool focused: false
    width: 15*dp
    height: 30*dp
	anchors.left: parent.left

    signal touched()

    Image {
        width: 30*dp
        height: 30*dp
        x: -15*dp
		source: "qrc:/images/node_shadow.png"
    }

    Rectangle {
        width: 14*dp
        height: 14*dp
        x: -7*dp
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

    BezierCurve {
		id: bezierCurve
		objectName: "bezierCurve"
        lineWidth: 3*dp
        color: Qt.rgba(0, 0.3, 1, 0.7)
        z: -1
        onLeftItemChanged: {
            update()
        }
        onRightItemChanged: {
            update()
        }
        function update() {
            if (!leftItem || !rightItem) {
                return
            }
			startPoint = leftItem.mapToItem(null, 15*dp, 15*dp)
			endPoint = rightItem.mapToItem(null, 0, 15*dp)
        }
    }

//	BezierCurveShader {
//		id: curve
//		objectName: "bezierCurve"
//		property Item leftItem
//		property Item rightItem

//		onLeftItemChanged: {
//			update()
//		}
//		onRightItemChanged: {
//			update()
//		}
//		function update() {
//			if (!leftItem || !rightItem) {
//				return
//			}
//			begin = leftItem.mapToItem(curve, 15*dp, 13*dp)
//			end = parent.mapToItem(curve, -7*dp, 13*dp)
//		}

//		begin: leftItem ? leftItem.mapToItem(this, 15*dp, 15*dp) : end
//		end: parent.mapToItem(this, 0, 0)
//	}

    TouchArea {
        id: touchArea
        width: 30*dp
        height: 30*dp
        x: -15*dp

        onTouchDown: {
            parent.touched()
        }
    }
}

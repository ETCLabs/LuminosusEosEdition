import QtQuick 2.5
import QtQuick.Window 2.0  // for Screen and HighDPI scaling
import QtGraphicalEffects 1.0

Item {
    property real imageSize: Math.min(width, height)
    property string iconName: ""
    property alias color: overlay.color

    Image {
        id: icon
        width: imageSize
        height: width
        anchors.centerIn: parent
        source: "qrc:/images/svg/" + iconName + ".svg"
        fillMode: Image.PreserveAspectFit
        smooth: true

        // render in double resolution to be antialiased and smooth:
        sourceSize.width: width * Screen.devicePixelRatio * 2
    }

    ColorOverlay {
        id: overlay
        anchors.fill: icon
        source: icon
        cached: true
        color: "#888"
    }

}

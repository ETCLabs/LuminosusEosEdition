import QtQuick 2.5

Rectangle {
    property alias startWhen: animation.running
    anchors.fill: parent
    color: "white"
    opacity: 0
    NumberAnimation on opacity {
        id: animation
        alwaysRunToEnd: true
        from: 0.2; to: 0
        duration: 400
        easing.type: Easing.OutCurve
    }
    function start() {
        animation.running = true
        animation.running = false
    }
}

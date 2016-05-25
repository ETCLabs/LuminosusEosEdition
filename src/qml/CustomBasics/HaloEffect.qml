import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    property Item source
    anchors.fill: parent
    visible: true

    GammaAdjust {
        id: gamma_shader
        anchors.fill: parent
        source: source
        gamma: 0.1
        visible: false
    }

    GaussianBlur {
        id: blur_shader
        anchors.fill: parent
        source: gamma_shader
        radius: 8
        samples: 16
        deviation: 3
        visible: false
    }

    GammaAdjust {
        id: bc_shader
        anchors.fill: parent
        source: blur_shader
        gamma: 1.3
        visible: false
    }

    Blend {
        anchors.fill: parent
        source: source
        foregroundSource: bc_shader
        mode: "addition"
        visible: true
    }
}


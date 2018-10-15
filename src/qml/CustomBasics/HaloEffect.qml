import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    property Item source
    anchors.fill: parent
    visible: true

    GammaAdjust {
        id: gamma_shader
        anchors.fill: parent
        source: parent.source
        gamma: 0.1
        visible: false
    }

    GaussianBlur {
        id: blur_shader
        anchors.fill: parent
        source: gamma_shader
        radius: 32
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
        source: parent.source
        foregroundSource: bc_shader
        mode: "addition"
        visible: true
    }

    // ------------- Halo -----------------

//    LevelAdjust {
//        id: threshold
//        anchors.fill: parent
//        source: content
//        minimumInput: "#99999999"
//        maximumInput: "#cccccccc"
//        visible: false
//    }

//    FastBlur {
//        id: halo
//        anchors.fill: parent
//        source: threshold
//        radius: 32
//        visible: false
//    }

//    Blend {
//        anchors.fill: parent
//        source: content
//        foregroundSource: halo
//        mode: "addition"
//        visible: true
//    }
}


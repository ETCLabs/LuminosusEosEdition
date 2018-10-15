import QtQuick 2.5
import CustomElements 1.0
import "CustomBasics"
import "CustomControls"
import QtGraphicalEffects 1.0

Item {
    width: 1920
    height: 1080

    Rectangle {
        id: spectrum
        width: 1920
        height: 400
        anchors.bottom: parent.bottom
        layer.enabled: true
        color: "black"
        SpectrumItem {
            width: 960
            height: 200
            x: 0
            points: controller.speechToTextManager().getDefaultSpeechInput().valueHistory
            color: "black"
        }
        SpectrumItem {
            width: 960
            height: 200
            x: 1920  // mirrored to the left
            points: controller.speechToTextManager().getDefaultSpeechInput().valueHistory
            color: "black"
            transform: Scale { xScale: -1 }
        }

        SpectrumItem {
            width: 960
            height: 200
            x: 0
            y: 400
            points: controller.speechToTextManager().getDefaultSpeechInput().valueHistory
            color: "black"
            transform: Scale { yScale: -1 }
        }
        SpectrumItem {
            width: 960
            height: 200
            x: 1920  // mirrored to the left
            y: 400
            points: controller.speechToTextManager().getDefaultSpeechInput().valueHistory
            color: "black"
            transform: Scale { xScale: -1; yScale: -1 }
        }
    }

    HaloEffect {
        anchors.fill: spectrum
        source: spectrum
    }
}

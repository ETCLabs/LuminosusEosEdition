import QtQuick 2.10
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

import "content.js" as Content

Item {

    Text {
        text: "Tutorials"
        font.pixelSize: 26*dp
        font.bold: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 40*dp
        horizontalAlignment: Text.AlignHCenter
    }

    ListView {
        id: listView
        anchors.fill: parent
        anchors.leftMargin: 120*dp
        anchors.rightMargin: 120*dp
        anchors.topMargin: 120*dp
        anchors.bottomMargin: 1
        footer: Item { height: 20*dp }
        spacing: 20*dp
        clip: true

        model: Content.tutorials

//       [
//            { category: "Basics", title: "Blocks + Connections", source: "" },
//            { category: "Basics", title: "Save & Load Projects", source: "" },
//            { category: "Connect", title: "Lighting Consoles", source: "" },
//            { category: "Connect", title: "Audio Consoles", source: "" },
//            { category: "Connect", title: "MIDI Controller", source: "" },
//            { category: "Connect", title: "OSC In & Out", source: "" },
//            { category: "Connect", title: "sACN Reception", source: "" },
//            { category: "Use Case", title: "Sound2Light", source: "" },
//            { category: "Use Case", title: "Remote Control", source: "" },
//            { category: "Use Case", title: "Media Playback", source: "" },
//            { category: "Use Case", title: "Cue List Editing / Monitor", source: "" },
//            { category: "Use Case", title: "Interactive Props", source: "" },
//        ]

        delegate: CustomTouchArea {
            width: listView.width
            height: 50*dp
            mouseOverEnabled: true

            onClick: stackView.push("qrc:/qml/Tutorial/SlideShow.qml", { tutorial: modelData })

            Rectangle {
                anchors.fill: parent
                color: "#444"
                border.color: parent.mouseOver ? "yellow" : "#aaa"
                border.width: 1
            }

            StretchRow {
                anchors.fill: parent
                anchors.margins: 1
                Rectangle {
                    width: 100*dp
                    color: "#555"
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 16*dp
                        text: modelData.category
                    }
                }
                Item {
                    implicitWidth: -1
                    Text {
                        anchors.centerIn: parent
                        font.pixelSize: 20*dp
                        font.bold: true
                        text: modelData.title
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }
    }

}

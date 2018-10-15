import QtQuick 2.5
import CustomElements 1.0
import QtGraphicalEffects 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 90*dp
    height: 150*dp
    settingsComponent: settings

    // ------------ Result Color Circle -------------------

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        y: -20*dp
        width: 2*dp
        height: 20*dp
        color: "#333"
    }
    Rectangle {
        id: colorCircle
        anchors.horizontalCenter: parent.horizontalCenter
        y: -48*dp

        width: 40*dp
        height: 40*dp
        radius: 20*dp
        color: block.attr("resultColor").qcolor
    }
    Image {
        id: glowImage
        anchors.centerIn: colorCircle
        width: 100*dp
        height: 100*dp
        source: "qrc:/images/light_glow.png"
        visible: false
    }
    ColorOverlay {
        anchors.fill: glowImage
        source: glowImage
        color: block.attr("resultColor").glow
        opacity:block.attr("resultColor").max
    }
    Image {
        anchors.fill: colorCircle
        source: "qrc:/images/light_glow_foreground.png"
        opacity: block.attr("resultColor").max * 0.4
    }

    // ------------ W + A + UV Circles ------------------

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 25*dp
        y: -20*dp

        width: 12*dp
        height: 12*dp
        radius: 6*dp
        color: Qt.hsva(0, 0, block.attr("resultWhite").val, 1)
    }
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 40*dp
        y: -20*dp

        width: 12*dp
        height: 12*dp
        radius: 6*dp
        color: Qt.hsva(0.065, 1.0, block.attr("resultAmber").val, 1)
    }
    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: -30*dp
        y: -20*dp

        width: 12*dp
        height: 12*dp
        radius: 6*dp
        color: Qt.hsva(0.73, 0.8, block.attr("resultUV").val, 1)
    }

    // ------------------ Main Column --------------------

    StretchColumn {
        anchors.fill: parent

        BlockRow {
            InputNode {
                node: block.node("inputNode")
                width: 15*dp
            }
            ButtonBottomLine {
                width: 30*dp
                implicitWidth: 0
                text: block.attr("benchEffects").val > 0.0 ? "E" : "S"
                onPress: block.toggleBenchEffects();
                mappingID: block.getUid() + block.attr("benchEffects").name()
            }
            AttributeDotColorPicker {
                width: 30*dp
                attr: block.attr("benchColor")
                mappingID: block.getUid() + "benchColor"
            }

            // ---------------------------
            OutputNode {
                node: block.node("outputNode")
                width: 15*dp
            }
        }

        BlockRow {
            InputNode {
                node: block.node("whiteIn")
                width: 15*dp
            }
            Text {
                width: 30*dp
                text: "W"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("benchWhite")
            }
            Item {
                width: 30*dp
                implicitWidth: 0
            }
            OutputNode {
                node: block.node("whiteOut")
                width: 15*dp
            }
        }

        BlockRow {
            InputNode {
                node: block.node("amberIn")
                width: 15*dp
            }
            Text {
                width: 30*dp
                text: "A"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("benchAmber")
            }
            Item {
                width: 30*dp
                implicitWidth: 0
            }
            OutputNode {
                node: block.node("amberOut")
                width: 15*dp
            }
        }

        BlockRow {
            InputNode {
                node: block.node("uvIn")
                width: 15*dp
            }
            Text {
                width: 30*dp
                text: "UV"
            }
            AttributeDotSlider {
                width: 30*dp
                implicitWidth: 0
                attr: block.attr("benchUV")
            }
            Item {
                width: 30*dp
                implicitWidth: 0
            }
            OutputNode {
                node: block.node("uvOut")
                width: 15*dp
            }
        }

        DragArea {
            id: dragarea
            text: block.label || block.attr("address").val.toFixed(0)
        }
    }  // end main column

    Component {
        id: settings
        StretchColumn {
            leftMargin: 15*dp
            rightMargin: 15*dp
            defaultSize: 30*dp

            BlockRow {
                StretchText {
                    text: "Address:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("address")
                }
            }

            BlockRow {
                StretchText {
                    text: "Gamma:"
                }
                AttributeNumericInput {
                    width: 60*dp
                    attr: block.attr("gamma")
                    decimals: 1
                }
            }

            BlockRow {
                StretchText {
                    text: "Current Effects:"
                }
                Text {
                    width: 50*dp
                    text: (block.attr("resultEffects").val * 100).toFixed(0) + "%"
                }
            }
        }
    }
}


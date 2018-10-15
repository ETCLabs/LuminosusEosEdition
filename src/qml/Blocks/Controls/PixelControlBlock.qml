import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: Math.max(180*dp, 30*dp * block.matrixWidth)
    height: 30*dp + 30*dp * block.matrixHeight

    property QtObject modificationMode: block.attr("modificationMode")

    Connections {
        target: block
        onMatrixSizeChanged: block.positionChangedExternal()
    }

    StretchColumn {
        anchors.fill: parent

        StretchColumn {
            implicitHeight: -1

            Repeater {
                model: block.matrixHeight

                StretchRow {
                    property int index: modelData
                    implicitHeight: -1

                    Repeater {
                        model: block.matrixWidth

                        Rectangle {
                            id: rect
                            property int mx: modelData
                            property int my: block.matrixHeight - parent.index - 1
                            implicitWidth: -1
                            implicitHeight: -1
                            color: Qt.hsva(block.getHue(mx, my), block.getSat(mx, my), block.getVal(mx, my), 1.0)
                            border.width: 1*dp
                            border.color: "#222"

                            Connections {
                                target: block
                                onMatrixChanged: rect.color = Qt.hsva(block.getHue(mx, my), block.getSat(mx, my), block.getVal(mx, my), 1.0)
                            }

                            CustomTouchArea {
                                anchors.fill: parent

                                onClick: {
                                    var prev, newVal
                                    if (modificationMode.val === 0) {
                                        prev = block.getHue(rect.mx, rect.my)
                                        newVal = (((prev + 0.1) % 1.0) + 1.0) % 1.0
                                        block.setHue(rect.mx, rect.my, newVal)
                                    } else if (modificationMode.val === 1) {
                                        prev = block.getSat(rect.mx, rect.my)
                                        newVal = (prev < 1.0) ? 1.0 : 0.0
                                        block.setSat(rect.mx, rect.my, newVal)
                                    } else {
                                        prev = block.getVal(rect.mx, rect.my)
                                        newVal = (prev < 1.0) ? 1.0 : 0.0
                                        block.setVal(rect.mx, rect.my, newVal)
                                    }

                                }

                                onTouchMove: {
                                    var prev, newVal
                                    if (modificationMode.val === 0) {
                                        prev = block.getHue(rect.mx, rect.my)
                                        newVal = (((prev - touch.deltaY / 300.0) % 1.0) + 1.0) % 1.0
                                        block.setHue(rect.mx, rect.my, newVal)
                                    } else if (modificationMode.val === 1) {
                                        prev = block.getSat(rect.mx, rect.my)
                                        newVal = Math.max(0.0, Math.min(prev - touch.deltaY / 200.0, 1.0))
                                        block.setSat(rect.mx, rect.my, newVal)
                                    } else {
                                        prev = block.getVal(rect.mx, rect.my)
                                        newVal = Math.max(0.0, Math.min(prev - touch.deltaY / 200.0, 1.0))
                                        block.setVal(rect.mx, rect.my, newVal)
                                    }
                                }
                            }
                        }  // end Rectangle
                    }  // end Rectangle Repeater
                }  // end row
            }  // end row Repeater
        }

        DragArea {
            text: ""
            BlockRow {
                height: 30*dp
                width: 4*30*dp
                ButtonSideLine {
                    text: "H"
                    onPress: modificationMode.val = 0
                    marked: modificationMode.val === 0
                    mappingID: block.getUid() + "h"
                }
                ButtonSideLine {
                    text: "S"
                    onPress: modificationMode.val = 1
                    marked: modificationMode.val === 1
                    mappingID: block.getUid() + "s"
                }
                ButtonSideLine {
                    text: "V"
                    onPress: modificationMode.val = 2
                    marked: modificationMode.val === 2
                    mappingID: block.getUid() + "v"
                }
                Item {
                    width: 30*dp
                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 3*dp
                        radius: width / 2
                        color: "transparent"
                        border.width: 1*dp
                        border.color: "#999"
                    }
                    Text {
                        anchors.centerIn: parent
                        text: "A"
                    }
                    CustomTouchArea {
                        anchors.fill: parent

                        onClick: {
                            var prev, newVal
                            if (modificationMode.val === 0) {
                                prev = block.getHue(0, 0)
                                newVal = (((prev + 0.1) % 1.0) + 1.0) % 1.0
                                block.setHueAll(newVal)
                            } else if (modificationMode.val === 1) {
                                prev = block.getSat(0, 0)
                                newVal = (prev < 1.0) ? 1.0 : 0.0
                                block.setSatAll(newVal)
                            } else {
                                prev = block.getVal(0, 0)
                                newVal = (prev < 1.0) ? 1.0 : 0.0
                                block.setValAll(newVal)
                            }
                        }

                        onTouchMove: {
                            var prev, newVal
                            if (modificationMode.val === 0) {
                                prev = block.getHue(0, 0)
                                newVal = (((prev - touch.deltaY / 300.0) % 1.0) + 1.0) % 1.0
                                block.setHueAll(newVal)
                            } else if (modificationMode.val === 1) {
                                prev = block.getSat(0, 0)
                                newVal = Math.max(0.0, Math.min(prev - touch.deltaY / 200.0, 1.0))
                                block.setSatAll(newVal)
                            } else {
                                prev = block.getVal(0, 0)
                                newVal = Math.max(0.0, Math.min(prev - touch.deltaY / 200.0, 1.0))
                                block.setValAll(newVal)
                            }
                        }
                    }
                }
            }
            OutputNode {
                node: block.node("outputNode")
                suggestions: ["Offset", "Vertical Patch"]
            }
        }
    }
}

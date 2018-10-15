import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: 300*dp
    height: 120*dp

    StretchColumn {
        anchors.fill: parent

        StretchRow {
            height: 90*dp

            Item {
                width: 90*dp
                height: 90*dp

                Image {
                    anchors.fill: parent
                    source: "qrc:/images/luminosus_info_block_logo.png"
                    smooth: true
                }

                Image {
                    anchors.fill: parent
                    source: "qrc:/images/etc_logo_180_circle_green.png"

                    RotationAnimator on rotation {
                        from: 0;
                        to: 360;
                        duration: 10 * 1000  // 10s per full rotation
                        loops: Animation.Infinite
                        running: true
                    }
                }
            }

            StretchColumn {
                implicitWidth: -1
                leftMargin: 10*dp

                BlockRow {
                    StretchText {
                        text: "Version: " + controller.getVersionString()
                        hAlign: Text.AlignLeft
                    }
                }
                BlockRow {
                    StretchText {
                        text: "Qt Runtime Ver.: " + block.getQtRuntimeVersion()
                        hAlign: Text.AlignLeft
                    }
                }
                BlockRow {
                    Text {
                        property real t
                        color: (fpsTimer.fps > 30) ? "white" : "#f22"
                        text: "Current FPS: " + fpsTimer.fps

                        Timer {
                            id: fpsTimer
                            property real fps: 0
                            property int frame: 0
                            repeat: true
                            interval: 1000
                            running: true
                            onTriggered: {
                                fps = frame
                                frame = 0
                            }
                        }

                        NumberAnimation on t {
                            id: tAnim
                            from: 0
                            to: 100
                            loops: Animation.Infinite
                        }

                        onTChanged: {
                            update() // force continuous animation
                            ++fpsTimer.frame
                        }
                    }
                }
            }  // end right text column
        }  // end content Row

        DragArea {
            text: "Luminosus Info"
        }
    }  // end main Column
}

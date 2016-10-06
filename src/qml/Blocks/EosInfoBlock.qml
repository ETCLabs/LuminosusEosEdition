import QtQuick 2.5
import CustomElements 1.0
import "../CustomBasics"
import "../CustomControls"

BlockBase {
    id: root
    width: 490*dp
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
                    source: "qrc:/images/etc_logo_180.png"
                    smooth: true
                }

                Image {
                    anchors.fill: parent
                    source: controller.eosManager().pingIsSuccessful
                            ? "qrc:/images/etc_logo_180_circle_green.png"
                            : "qrc:/images/etc_logo_180_circle_red.png"

                    RotationAnimator on rotation {
                        from: 0;
                        to: 360;
                        duration: 10 * 1000  // 10s per full rotation
                        loops: Animation.Infinite
                        running: (controller.eosManager().latency > 0)
                                 && (controller.eosManager().latency < 200)
                    }
                }
            }

            StretchColumn {
                implicitWidth: -1
                leftMargin: 10*dp

                BlockRow {
                    StretchText {
                        id: ipLabel
                        implicitWidth: -0.6
                        text: "IP: " + controller.eosConnection().getIpAddressString()
                        hAlign: Text.AlignLeft
                        Connections {
                            target: controller.eosConnection()
                            onAddressChanged: ipLabel.text = "IP: " + controller.eosConnection().getIpAddressString()
                        }
                    }
                    StretchText {
                        implicitWidth: -0.4
                        text: "Latency: " + controller.eosManager().latency + "ms"
                        hAlign: Text.AlignLeft
                        visible: controller.eosManager().pingIsSuccessful
                    }
                    StretchText {
                        implicitWidth: -0.4
                        text: "Timeout"
                        color: "#FF3634"
                        hAlign: Text.AlignLeft
                        visible: !controller.eosManager().pingIsSuccessful
                    }
                }

                BlockRow {
                    StretchText {
                        implicitWidth: -0.6
                        text: "Version: " + controller.eosManager().consoleVersion
                        hAlign: Text.AlignLeft
                        visible: controller.eosManager().pingIsSuccessful
                    }
                    StretchText {
                        implicitWidth: -0.4
                        text: "OSC User: " + controller.eosManager().oscUserId
                        hAlign: Text.AlignLeft
                        visible: controller.eosManager().pingIsSuccessful
                    }
                }

                BlockRow {
                    StretchText {
                        text: "Show: " + controller.eosManager().showTitle
                        hAlign: Text.AlignLeft
                        visible: controller.eosManager().pingIsSuccessful
                    }
                }
            }  // end right text column
        }  // end content Row

        DragArea {
            text: "Eos Info"
        }
    }  // end main Column
}

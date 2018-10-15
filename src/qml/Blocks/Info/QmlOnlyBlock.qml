import QtQuick 2.5
import CustomElements 1.0
import "../../CustomBasics"
import "../../CustomControls"

BlockBase {
    id: root
    width: block.qmlFilePath ? Math.max(loader.width*dp, 30*dp) : 160*dp
    height: block.qmlFilePath ? Math.max(loader.height*dp + 30*dp, 60*dp) : 60*dp

    StretchColumn {
        anchors.fill: parent

        ButtonBottomLine {
            height : 30*dp
            text: "Select QML File"
            visible: block.qmlFilePath === ""
            onPress: {
                qmlFileDialogLoader.source = ""
                qmlFileDialogLoader.source = "qrc:/qml/QmlOnlyBlockFileSelectorDialog.qml"
            }

            Loader {
                id: qmlFileDialogLoader
            }
        }

        Item {
            implicitHeight: -1
            Loader {
                id: loader
                anchors.centerIn: parent
                scale: dp
                source: block.qmlFilePath
                visible: block.qmlFilePath !== ""
            }
        }

        DragArea {
            text: block.fileBaseName || "Custom QML"
        }
    }  // end main Column
}


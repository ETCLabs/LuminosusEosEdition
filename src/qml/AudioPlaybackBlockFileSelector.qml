import QtQuick 2.5
import QtQuick.Dialogs 1.2

FileDialog {
    id: importDialog
    title: "Select Audio File"
    folder: shortcuts.music
    selectMultiple: false
    nameFilters: "Audio Files (*)"
    onAccepted: {
        if (fileUrl) {
            block.filePath = fileUrl
        }
    }
    onRejected: {
        // console.log("Canceled")
    }
    Component.onCompleted: {
        // don't set visible to true before component is complete
        // because otherwise the dialog will not be configured correctly
        visible = true
    }
}

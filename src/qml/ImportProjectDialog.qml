import QtQuick 2.5
import QtQuick.Dialogs 1.2

FileDialog {
    id: importDialog
    title: "Import Project"
    folder: shortcuts.home
    selectMultiple: false
    nameFilters: controller.projectManager().getFilenameFilters()
    onAccepted: {
        if (fileUrl) {
            controller.projectManager().importProjectFile(fileUrl)
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

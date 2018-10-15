import QtQuick 2.0

Item {
    id: root
    height: 280*dp

    property QtObject oscManager
    property bool newPresetAreaIsOpen: false
    property bool showSpecificStatus: true

    OscPresetArea {
        visible: !newPresetAreaIsOpen
        anchors.fill: parent
        oscManager: root.oscManager
        onOpenNewPresetArea: {
            newPresetAreaIsOpen = true
            newPresetLoader.item.originalPresetId = originalPresetId
        }
        showSpecificStatus: root.showSpecificStatus
    }

    Loader {
        id: newPresetLoader
        active: newPresetAreaIsOpen
        anchors.fill: parent
        sourceComponent: NewOscPresetArea {
            oscManager: root.oscManager
            onClosed: newPresetAreaIsOpen = false
        }
    }
}

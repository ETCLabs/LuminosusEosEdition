import QtQuick 2.5
import QtQuick.Window 2.2

// Can be used to show a component as an "overlay". The overlay item will be the last children
// of the window item so that it is drawn above all other items and always visible.

Item {

    property Component sourceComponent
    property Item item

    function show(preferredLocalX, prefferedLocalY) {
        if (item) {
            if (!item.visible) {
                item.visible = true
            } else {
                console.log("Overlay is already visible.")
            }
            return
        }

        // ---------- Translate Local Coordinates to Window Coordiantes -----------
        var windowCoords = root.mapToItem(null, preferredLocalX, prefferedLocalY)
        var winX = windowCoords.x
        var winY = windowCoords.y

        // -------------------- Show Component -------------------
        var window = Window.contentItem
        item = sourceComponent.createObject(window, {x: winX, y: winY})
    }

    function hide() {
        if (item) {
            if (item.visible) {
                item.visible = false
            } else {
                console.log("Overlay item is already hidden.")
            }
            return
        } else {
            console.log("Overlay item doesn't exist (hide).")
        }
    }

    function destroyItem() {
        if (item) {
            item.destroy()
            return
        } else {
            console.log("Overlay item doesn't exist (destroy).")
        }
    }

}

import QtQuick 2.0

Item {
    id: root

    property real scaleOriginX: 0.0
    property real scaleOriginY: 0.0
    property real customScale: 1.0

    transform: Scale {
        origin.x: scaleOriginX
        origin.y: scaleOriginY
        xScale: customScale
        yScale: customScale
    }

    // Blocks will be added here
}


import QtQuick 2.0
import "ShaderResources.js" as Resources

ShaderEffect {
    property real time : 0.0
    NumberAnimation on time { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 6283 }
    property real resolution_x: width
    property real resolution_y: height

    fragmentShader: Resources.greyWave
}


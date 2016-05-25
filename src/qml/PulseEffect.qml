import QtQuick 2.2
ShaderEffect {
    id: shaderEffect

    // Properties that will get bound to a uniform with the same name in the shader
    property color backgroundColor: "#00000000"
    property color baseColor: "#fff"
    // Our animated uniform property
    property real time: 0
    opacity: 1

    ParallelAnimation {
        running: true
        UniformAnimator {
            uniform: "time"; target: shaderEffect
            from: 0; to: 1
            duration: 3000
            loops: Animation.Infinite
        }
    }

    fragmentShader: "
        varying mediump vec2 qt_TexCoord0;
        uniform lowp float qt_Opacity;
        uniform lowp vec4 backgroundColor;
        uniform lowp vec4 baseColor;
        uniform mediump float time;

        void main() {
            float radius = length(qt_TexCoord0 - vec2(0.5, 0.5));
            float variation = 0.8 + sin(time*3.1415) * 0.2;
            float brightness = pow(max(0.5 - radius, 0.0), 0.5) * variation;
            lowp vec4 glow = baseColor * brightness;
            gl_FragColor = (backgroundColor + glow) * qt_Opacity;
        }
    "
}

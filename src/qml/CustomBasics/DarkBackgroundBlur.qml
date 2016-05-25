import QtQuick 2.5
import QtGraphicalEffects 1.0

Item {
	id: root
	property Item blurSource: parent
	property rect blurArea: Qt.rect(x, y, width, height)
	property real brightness: -0.7
	property real contrast: -0.3
	property color alternativeColor: Qt.rgba(0.15, 0.15, 0.15, 1)

	FastBlur {
		id: blur
		anchors.fill: parent
		radius: 32*dp
		visible: GRAPHICAL_EFFECTS_LEVEL >= 3

		source: ShaderEffectSource {
			sourceItem: blurSource
			sourceRect: blurArea
		}
	}
	BrightnessContrast {
		anchors.fill: parent
		source: blur
		brightness: root.brightness
		contrast: root.contrast
		visible: GRAPHICAL_EFFECTS_LEVEL >= 3
	}
	Rectangle {
		// blur alternative on low graphics
		anchors.fill: parent
		color: alternativeColor
		visible: GRAPHICAL_EFFECTS_LEVEL < 3
	}
}

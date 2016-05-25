import QtQuick 2.5

Item {
	property point begin: Qt.point(0, 0)
	property point end: Qt.point(1, 1)
	property Item reference: parent

	property alias lineColor: shaderEffect.lineColor
	property alias lineWidth: shaderEffect.lineColor


	ShaderEffect {
		id: shaderEffect

		x: begin.x
		y: Math.min(begin.y, end.y)

		width: end.x - begin.x
		height: Math.abs(end.y - begin.y)

		property color lineColor: "blue"
		property real lineWidth: 0.015
		property real resolutionX: width
		property real resolutionY: height

		property bool flip: begin.y > end.y

		fragmentShader: "
			varying mediump vec2 qt_TexCoord0;
			uniform lowp float qt_Opacity;
			uniform lowp vec4 lineColor;
			uniform mediump float lineWidth;
			uniform mediump float resolutionX;
			uniform mediump float resolutionY;
			uniform bool flip;

			float det(vec2 a, vec2 b) { return a.x*b.y-b.x*a.y; }

			vec2 get_distance_vector(vec2 b0, vec2 b1, vec2 b2) {
				float a=det(b0,b2), b=2.0*det(b1,b0), d=2.0*det(b2,b1);
				float f=b*d-a*a;
				vec2 d21=b2-b1, d10=b1-b0, d20=b2-b0;
				vec2 gf=2.0*(b*d21+d*d10+a*d20);
				gf=vec2(gf.y,-gf.x);
				vec2 pp=-f*gf/dot(gf,gf);
				vec2 d0p=b0-pp;
				float ap=det(d0p,d20), bp=2.0*det(d10,d0p);

				float t=clamp((ap+bp)/(2.0*a+b+d), 0.0, 1.0);
				return mix(mix(b0,b1,t),mix(b1,b2,t),t);
			}

			float approx_distance(vec2 p, vec2 b0, vec2 b1, vec2 b2) {
				return length(get_distance_vector(b0-p, b1-p, b2-p));
			}


			void main() {

				float pixelX = 1.0 / resolutionX;
				float pixelY = 1.0 / resolutionY;

				float EDGE = 1 * pixelX;
				float SMOOTH = 1 * pixelX;
				float SHADOW = 5 * pixelX;

				float dist;

				if (qt_TexCoord0.y < 0.5) {

					vec2 A = flip ? vec2(1.0,0.0) : vec2(0.0,0.0);
					vec2 B = vec2(0.5,0.0);
					vec2 C = vec2(0.5,0.5);

					dist = approx_distance(qt_TexCoord0, A, B, C);

				} else {

					vec2 A = vec2(0.5,0.5);
					vec2 B = vec2(0.5,1.0);
					vec2 C = flip ? vec2(0.0,1.0) : vec2(1.0,1.0);

					dist = approx_distance(qt_TexCoord0, A, B, C);
				}
				lowp vec4 line = lineColor * smoothstep(dist - SMOOTH, dist + SMOOTH, EDGE);
				line = max(line, vec4(0, 0, 0, 0.3) * smoothstep(dist - SHADOW, dist + SHADOW, 0));


				gl_FragColor = line * qt_Opacity;
			}
		"
	}
}

attribute vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;
uniform vec4 uColor;
uniform float uSize;

void main(void){
	gl_PointSize = uSize;
	gl_Position = vec4(aPosition, 1.0) * uModel * uView * uProj;
}

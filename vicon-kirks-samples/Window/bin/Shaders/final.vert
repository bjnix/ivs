attribute vec3 aPosition;
attribute vec3 aColor;

uniform mat4 uView;
uniform mat4 uProj;

varying vec3 vColor;

void main(void){
	vColor = aColor;
	gl_Position = vec4(aPosition, 1.0) * uView * uProj;
}

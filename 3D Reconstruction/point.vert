#version 150

in vec3 vert;
in vec3 normal;
in vec2 texCoord;


out vec2 texCoordFrag;
out vec3 exNormal;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 scaleTrans;

void main(void){

	mat3 normalMatrix = mat3(mdlMatrix);

	exNormal = normal * normalMatrix;
	texCoord = texCoordFrag;
	gl_Position = projMatrix * mdlMatrix * scaleTrans * camMatrix * vec4(vert, 1.0);
}
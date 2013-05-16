#version 150

in vec3 vert;
in vec3 normal;
in vec2 texCoord;


out vec2 texCoordFrag;
out vec3 exNormal;
out vec3 exPosition;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 scaleTrans;
uniform mat4 objectRot;
uniform mat4 cameraRotation;

void main(void){

	mat3 normalMatrix = mat3(mdlMatrix);

	exNormal = normal * normalMatrix;
	texCoordFrag = texCoord;
	exPosition = vec3(scaleTrans * objectRot * vec4(vert, 1.0));
	gl_Position =  projMatrix * mdlMatrix * cameraRotation * camMatrix * scaleTrans * objectRot * vec4(vert, 1.0);
}
#version 150

in vec2 texCoordFrag;
in vec3 exNormal;

out vec4 outColor;

uniform sampler2D tex;

void main(void){

	const vec3 light = vec3(0.5,1.0,0.5);
	//const vec4 color = vec4(1.0,0.5,1.0,1.0);
	
	float ambientCoeff = 0.5;
	float diffuseCoeff = 0.5;

	float diffuseComponent = dot(normalize(exNormal),normalize(light));

	
	vec4 color = normalize(texture(tex,texCoordFrag));
	outColor = (ambientCoeff + diffuseCoeff*diffuseComponent) * color;
}
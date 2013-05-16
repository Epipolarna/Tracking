#version 150

in vec2 texCoordFrag;
in vec3 exNormal;
in vec3 exPosition;

out vec4 outColor;

uniform vec3 cameraPosition;

uniform sampler2D Tex0;

void main(void){

	const vec3 lightPosition = vec3(0.5,1.0,0.5);
	//const vec4 color = vec4(1.0,0.5,1.0,1.0);
	
	float ambientCoeff = 0.4;
	float diffuseCoeff = 0.3;
	float specularCoeff = 0.3;
	float specularExponent = 10;

	//float diffuseComponent = dot(normalize(exNormal),normalize(light));
	
	//////////////
	
	// Phong
	//vec3 normal = normalize(normalTransform * exNormal);
	vec3 normal = normalize(exNormal);
	
	vec3 lightDirection = normalize(lightPosition - exPosition);
	
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    vec3 cameraDirection = normalize(cameraPosition - exPosition);

    //float diffuseComponent = max(dot(normal, lightDirection), 0) * shadow;
    //float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent) * shadow;
	
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	
	float totalLight = (ambientCoeff + (diffuseCoeff*diffuseComponent + specularCoeff*specularComponent) );
	
	
	//vec3 color = vec3(1, 1, 1);
	
	//out_Color = vec4(totalLight, 1);
	
	/////////////

	
	vec4 color = normalize(texture(Tex0,texCoordFrag));
	color = color.bgra;
	//vec4 color = vec4(1, 0, 1, 1);
	outColor = totalLight * color;
}
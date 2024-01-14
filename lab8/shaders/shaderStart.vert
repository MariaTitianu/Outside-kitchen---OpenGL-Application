#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormalEye;
out vec4 fPos;

flat out vec4 flat_fPos;
flat out vec3 flat_fNormalEye;
flat out vec4 flat_fPosEye;
flat out vec2 flat_fTexCoords;

out vec4 fPosEye;
out vec2 fTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;

out vec2 fragTexCoords;

void main() 
{
	//compute eye space coordinates
	fPos = model * vec4(vPosition, 1.0f);
	fPosEye = view * model * vec4(vPosition, 1.0f);
	fNormalEye = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	fragTexCoords = vTexCoords;
	//flat
	flat_fPos = fPos;
	flat_fPosEye = fPosEye;
	flat_fNormalEye = fNormalEye;
	flat_fTexCoords = fTexCoords;
}
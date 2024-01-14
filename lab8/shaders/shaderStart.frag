#version 410 core
#define lightNum 6

in vec3 fNormalEye;
in vec4 fPos;
in vec4 fPosEye;
in vec2 fTexCoords;

flat in vec4 flat_fPos;
flat in vec3 flat_fNormalEye;
flat in vec4 flat_fPosEye;
flat in vec2 flat_fTexCoords;

out vec4 fColor;

//lighting
uniform	vec3 sunlightDir;
uniform	vec3 sunlightColor;
uniform float sunlightStrength;

uniform int lightEnabled[lightNum];
uniform vec3 lightPosition[lightNum];
uniform vec3 lightDirection[lightNum];
uniform vec3 lightColor[lightNum];
uniform float lightFOV[lightNum];
uniform float lightStrength[lightNum];
uniform float ambientStrengthTexture;
uniform vec3 ambientColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//transparency
uniform float alpha;

//flat 
uniform int flatness;


vec3 ambient;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;
float shadow;


void computeLightComponents(vec3 fragment_normal, vec4 fragment_position_eye, vec4 fragment_position) //TODO unifica functiile
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fragment_normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(sunlightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragment_position_eye.xyz);

	//compute ambient light
	ambient = sunlightStrength * sunlightColor;
	
	//compute diffuse light
	diffuse = sunlightStrength * max(dot(normalEye, lightDirN), 0.0f) * sunlightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = sunlightStrength * specularStrength * specCoeff * sunlightColor;

	for(int i = 0; i < lightNum; i++){
		if(lightEnabled[i] == 1){
			//compute light direction
			lightDirN = normalize(lightPosition[i] - fragment_position_eye.xyz);
	
			//compute distance to light
			float dist = length(lightPosition[i] - fragment_position.xyz);

			float att = 1.0f;


			float angleToDirection = acos(dot(normalize(lightPosition[i] - fragment_position.xyz), lightDirection[i]));
			if(angleToDirection <= radians(lightFOV[i] / 2.0f)){
				//compute ambient light
				ambient += att * lightStrength[i] * lightColor[i];
	
				//compute diffuse light
				diffuse += att * lightStrength[i] * max(dot(normalEye, lightDirN), 0.0f) * lightColor[i];
	
				//compute specular light
				vec3 reflection = reflect(-lightDirN, normalEye);
				float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
				specular += att * lightStrength[i] * specularStrength * specCoeff * lightColor[i];
			}
		}
	}
}


void main() 
{
	if(texture(diffuseTexture, fTexCoords).a < 0.1){
		discard;
	}
	if(ambientColor.r != 0 || ambientColor.g != 0 || ambientColor.b != 0){
		computeLightComponents(fNormalEye, fPosEye, fPos);
		ambient *= ambientStrengthTexture * ambientColor;
		diffuse *= ambientColor;
		specular *= ambientColor;
	}
	else if(flatness == 2){
		computeLightComponents(flat_fNormalEye, flat_fPosEye, flat_fPos);
		ambient *= ambientStrengthTexture * texture(diffuseTexture, flat_fTexCoords).rgb;
		diffuse *= texture(diffuseTexture, flat_fTexCoords).rgb;
		specular *= texture(specularTexture, flat_fTexCoords).rgb;
	}
	else if(flatness == 1){
		computeLightComponents(flat_fNormalEye, fPosEye, fPos);
		ambient *= ambientStrengthTexture * texture(diffuseTexture, fTexCoords).rgb;
		diffuse *= texture(diffuseTexture, fTexCoords).rgb;
		specular *= texture(specularTexture, fTexCoords).rgb;
	}
	else{
		computeLightComponents(fNormalEye, fPosEye, fPos);
		ambient *= ambientStrengthTexture * texture(diffuseTexture, fTexCoords).rgb;
		diffuse *= texture(diffuseTexture, fTexCoords).rgb;
		specular *= texture(specularTexture, fTexCoords).rgb;
	}

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	//vec3 color = min((ambient + diffuse) + specular, 1.0f);
    
	vec4 color4 = vec4(color, alpha);
	
	fColor = color4;
	
}

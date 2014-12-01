#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vt;
layout (location = 3) in vec4 tangent;

//matrix
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normalMatrix;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

//out variabels
out vec2 textureCoordinates;
out vec3 viewTangentSpace;
out vec3 lightTangentSpace; 

void main() 
{
	gl_Position = proj * view * vec4 (vertexPosition, 1.0);
	textureCoordinates = vt;

	//calc the info on world space
	vec4 worldPosition = normalMatrix * vec4(vertexPosition, 1);
	vec3 worldNormal = normalize(normalMatrix * vec4(vertexNormal, 0)).xyz;
	vec3 worldTangent = normalize(normalMatrix * tangent).xyz;
	vec3 worldBinormal =  cross(worldNormal, worldTangent) * tangent.w;
	vec3 worldDirectionVertexPositionToLight = normalize(lightPosition - worldPosition.xyz);
	vec3 worldDirectionVertexPositionToCamera = normalize(cameraPosition - worldPosition.xyz);

	//convert the view to tangent space
	viewTangentSpace = vec3 (
		dot( worldDirectionVertexPositionToCamera, worldTangent), 
		dot( worldDirectionVertexPositionToCamera, worldBinormal), 
		dot( worldDirectionVertexPositionToCamera, worldNormal)
	);
	
	//converte the light to tangent space
	lightTangentSpace = vec3 (
		dot( worldDirectionVertexPositionToLight, worldTangent),
		dot( worldDirectionVertexPositionToLight, worldBinormal),
		dot( worldDirectionVertexPositionToLight, worldNormal)
	);

}

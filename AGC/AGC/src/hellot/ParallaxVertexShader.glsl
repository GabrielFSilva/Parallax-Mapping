#version 400

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 vt;
layout (location = 3) in vec4 tangent;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normalMatrix;

//positions
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

//out variables
out vec2 textureCoordinates;
out vec3 viewTangentSpace;
out vec3 lightTangentSpace; 
out vec4 tan;

void main() 
{
	//position the vertex
	gl_Position = proj * view * vec4 (vertexPosition, 1.0);
	
	//pass information to the fragment shader
	textureCoordinates = vt;
	tan = tangent;
	
	// calc the camera position and light direction in the world space
	vec3 cameraPositionWorld = (inverse (view) * vec4 (0.0, 0.0, 0.0, 1.0)).xyz;;
	vec3 lightDirectionWorld = vec3 (0.0, 0.0, -1.0);
	
	// bitangent calc
	vec3 bitangent = cross (vertexNormal, tangent.xyz) * tangent.w;
	
	// calc the camera position and light direction in the local space
	vec3 cameraPositionLocal = vec3 (inverse (model) * vec4 (cameraPositionWorld, 1.0));
	vec3 lightDirectionLocal = vec3 (inverse (model) * vec4 (lightDirectionWorld, 0.0));

	// view diection calc
	vec3 viewDirectionLocal = normalize (cameraPositionLocal - vertexPosition);
	
	//view matrix in tangent space (TBN Matrix)
	viewTangentSpace = vec3 (
		dot (tangent.xyz, viewDirectionLocal),
		dot (bitangent, viewDirectionLocal),
		dot (vertexNormal, viewDirectionLocal)
	);
	
	//light position in tangent space
	lightTangentSpace = vec3 (
		dot (tangent.xyz, lightDirectionLocal),
		dot (bitangent, lightDirectionLocal),
		dot (vertexNormal, lightDirectionLocal)
	);
}

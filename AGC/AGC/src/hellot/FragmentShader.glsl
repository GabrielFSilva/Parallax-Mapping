#version 400

in vec3 vertex;
in vec3 normal;

out vec4 frag_colour;

void main() 
{
	vec3 lightPosition =  vec3(0.0f,0.5f,1.0f);
	vec3 lightDiffuse =  vec3(0.25f,0.05f,0.75);

	vec3 viewerPositon = vec3(0.0f,0.0f,2.0f);

	float shininess = 5.0;

	vec3 surf2light = normalize(lightPosition-vertex);
	vec3 surf2view = normalize(viewerPositon-vertex);

	vec3 normalizedNormal = normalize(normal);
	vec3 reflection = reflect(-surf2light,normalizedNormal);

	float diffuseContribution = min(max(0.0,dot(normalizedNormal,surf2light)),1.0f);
	float specularConttribution = pow(max(0.0,dot(reflection,surf2view)),shininess);

	vec3 diffuse = diffuseContribution*lightDiffuse + lightDiffuse*specularConttribution;
	
	frag_colour = vec4(diffuse,1.0f);

}

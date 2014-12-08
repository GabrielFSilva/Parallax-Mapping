#version 400

//in variables
in vec2 textureCoordinates;
in vec3 viewTangentSpace;
in vec3 lightTangentSpace;

//texture sampler
uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform sampler2D textureHeight;

//Parallax info
uniform vec2 scaleBias;

//exit color
out vec4 frag_colour;

//ligth variables
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

//material variables
uniform vec4 materialAmbient;
uniform vec4 materialDiffuse;
uniform vec4 materialSpecular;
uniform float materialShininess;

void main() 
{
	//texture coor after parallax
	vec2 newTexCoord;
	
	//height calc
	float height = texture2D(textureHeight, textureCoordinates).r;
	height = height * scaleBias.x + scaleBias.y;
	
	//find the new texCoords
	vec3 h = normalize(viewTangentSpace);
    newTexCoord = textureCoordinates + (height * h.xy);
	
	//normals converted to the tangent space
	vec3 normal_tan = texture (textureNormal,newTexCoord).rgb;
	normal_tan = normalize (normal_tan * 2.0 - 1.0);
	
	//------- Light Calculation in Tanget Space --------//
	//Ambient
	vec3 Ia = materialAmbient.xyz;
	
	//Diffuse
	vec3 direction_to_light_tan = normalize (-lightTangentSpace);
	float dot_prod = dot (direction_to_light_tan, normal_tan);
	dot_prod = max (dot_prod, 0.0);
	vec3 Id = materialDiffuse.xyz * dot_prod;

	//Specular
	vec3 reflection_tan = reflect (normalize (lightTangentSpace), normal_tan);
	float dot_prod_specular = dot (reflection_tan, normalize (viewTangentSpace));
	dot_prod_specular = max (dot_prod_specular, 0.0);
	float specular_factor = pow (dot_prod_specular, materialShininess);
	vec3 Is = materialSpecular.xyz * specular_factor;

	//----- Output -------/
	frag_colour.rgb = (Is + Id + Ia) * texture2D(textureDiffuse, newTexCoord).xyz;
	frag_colour.a = 1.0;
}
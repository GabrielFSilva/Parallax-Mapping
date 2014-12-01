#version 400

//in variables
in vec2 textureCoordinates;
in vec3 viewTangentSpace;
in vec3 lightTangentSpace;

//texture sampler
uniform sampler2D textureDiffuse;
uniform sampler2D textureNormal;
uniform sampler2D textureHeight;

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
	//variabels for the parallax
	float scale = 1.0f;
	float bias = 0.15f;

	//texture coor after parallax
	vec2 newTexCoord;
	
	vec3 h = normalize(viewTangentSpace);

	//hight calc
	float height = texture2D(textureHeight, textureCoordinates).r;
	height = height * scale + bias;
    newTexCoord = textureCoordinates + (height * h.xy);

	//new normals based on the parallax
	vec3 n = normalize(texture2D(textureNormal, newTexCoord).rgb * 2.0 );
    vec3 l = normalize(lightTangentSpace);

	float nDotL = max(0.0, dot(n, l));
    float nDotH = max(0.0, dot(n, h));

	float power;
	
	if(nDotL == 0){
		power = 0.0;
	} else {
		pow(nDotH, materialShininess);
	}

	//Light Calc
	vec4 ambient = materialAmbient;
    vec4 diffuse = materialDiffuse * nDotL;
    vec4 specular = materialSpecular * power;
    vec4 color = ambient + diffuse + specular;
	
	frag_colour = texture2D(textureDiffuse, newTexCoord);
	//frag_colour = color * texture2D(textureDiffuse, newTexCoord);
}
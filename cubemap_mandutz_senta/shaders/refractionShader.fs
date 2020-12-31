#version 330 core
out vec4 FragColor;


in vec3 FragPos;
in mat3 NormalMatrix;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D normalMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{    
    vec3 normal;
    if( texture(normalMap, TexCoords).r != 0)
    {
        normal = texture(normalMap, TexCoords).rgb;         // obtain normal from normal map in range [0,1]
        normal = normalize(normal * 2.0 - 1.0);             // transform normal vector to range [-1,1]
        normal = NormalMatrix * normal;
    }
    else
    {
        normal = normalize(Normal);
    }
   
    float ratio = 1.00 / 1.309;
	vec3 I = normalize(FragPos - viewPos);
	vec3 R = refract(I, normal, ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
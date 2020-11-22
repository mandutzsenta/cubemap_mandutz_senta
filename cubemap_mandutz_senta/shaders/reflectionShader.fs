#version 330 core
out vec4 FragColor;


in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{           
    vec3 normal = texture(normalMap, TexCoords).rgb;                // obtain normal from normal map in range [0,1]
    normal = normalize(normal * 2.0 - 1.0);                         // transform normal vector to range [-1,1]
  
    vec3 I = normalize(FragPos - viewPos);
	vec3 R = reflect(I, normal);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
#version 330 core							//fragment shader
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D normalMap;

in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	vec3 normal = texture(normalMap, TexCoords).rgb;										//obtain normal from normal map in range [0,1]
    normal = normalize(normal * 2.0 - 1.0);													//transform normal vector to range [-1,1]	

	vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;				//ambient
																							//diffuse
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	vec3 viewDir = normalize(viewPos - FragPos);											//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}
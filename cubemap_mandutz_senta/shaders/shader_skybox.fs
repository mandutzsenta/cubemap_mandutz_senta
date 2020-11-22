#version 330 core								//fragment shader
out vec4 FragColor;

in vec3 TexCoords;								//vec3 instead of vec2

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);		//access cubemap texture
}
#version 330 core										//vertex shader
layout (location = 0) in vec3 aPos;						//position has attribute position 0

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	TexCoords = aPos;									//texture coordinate is set on the position vector
	vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;								//z component equal to 1.0 (maximum depth value)
}

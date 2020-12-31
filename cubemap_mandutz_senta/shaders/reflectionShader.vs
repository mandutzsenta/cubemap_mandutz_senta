#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 NormalMatrix;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));   
    TexCoords = aTexCoords;
    
    Normal = mat3(transpose(inverse(model))) * aNormal;

    NormalMatrix = transpose(inverse(mat3(model)));

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
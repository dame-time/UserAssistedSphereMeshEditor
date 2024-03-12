#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 ViewDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition = model * vec4(aPos, 1.0);
    
    gl_Position = projection * view * worldPosition;
    
    FragPos = vec3(worldPosition.xyz);
    Normal = aNormal;
    ViewDir = normalize(vec3(view[0][2], view[1][2], view[2][2]));
}

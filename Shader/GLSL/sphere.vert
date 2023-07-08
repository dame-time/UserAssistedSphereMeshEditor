#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float radius;
uniform vec3 center;

void main()
{
    vec4 worldPosition = vec4(aPos * radius + center, 1.0);
    
    gl_Position = projection * view * worldPosition;
    
    FragPos = vec3(worldPosition.xyz);
    Normal = aPos;
}

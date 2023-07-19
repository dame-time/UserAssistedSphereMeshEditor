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
    vec3 worldPosition = aPos * radius + center;
    
    gl_Position = projection * view * vec4(worldPosition,1.0);
    
    FragPos = vec3(worldPosition.xyz);
    Normal = aPos;
}

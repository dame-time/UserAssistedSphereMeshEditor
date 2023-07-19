#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 center;
uniform float radius;

out vec2 TexCoords;

void main()
{
    TexCoords = aPos * 0.5 + 0.5;
    
    vec4 worldPos = view * vec4(center, 1.0) + vec4(aPos * radius, 0.0, 1.0);
    
    gl_Position = projection * vec4(worldPos.xyz, 1.0);
}

#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 center;
uniform float radius;

out vec2 TexCoords;
out vec4 worldPos;
out vec3 ViewDir;
flat out float radiusClip;

void main()
{
    TexCoords = aPos;
    
    worldPos = view * vec4(center, 1.0) + vec4(aPos * radius, 0.0, 1.0);
    
//    vec4 projectedCenter = projection * view * vec4(center, 1.0);
//    vec4 projectedTip = projection * view * vec4(center + vec3(0, 0, radius), 1.0);
    
//    projectedCenter /= projectedCenter.w;
//    projectedTip /= projectedTip.w;
    
//    radiusClip = abs(projectedCenter.z - projectedTip.z) / 2;
    
    radiusClip = radius * projection[2][2] * 0.5;
    
    gl_Position = projection * vec4(worldPos.xyz, 1.0);
    gl_Position.w = 1;

    ViewDir = normalize(vec3(view[0][2], view[1][2], view[2][2]));
}

#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 color;

void main()
{
    vec2 pos = TexCoords * 2.0 - 1.0;

    // Calculate the squared distance from the center
    float distSqr = dot(pos, pos);

    // If the fragment is outside of the circle (distSqr > 1), discard the fragment
    if (distSqr > 1.0)
        discard;

    FragColor = vec4(color, 1.0);
}

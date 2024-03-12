#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoords;
in vec4 worldPos;
in vec3 ViewDir;
flat in float radiusClip;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 sphereCenter;

uniform mat4 view;
uniform mat4 projection;
uniform float radius;

void main()
{
    vec2 pos = TexCoords;

    float distSqr = dot(pos, pos);

    if (distSqr > 1.0)
        discard;
    
    float normalZ = sqrt(1.0 - distSqr);

    vec3 normal = vec3(pos, normalZ);

    // TODO: Light Dir diventa costante
    // Light direction
    vec3 lightDir = normalize(light.position - sphereCenter);

    // Ambient component
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(ViewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Final color
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
    
    gl_FragDepth = gl_FragCoord.z + normalZ * radiusClip;
}

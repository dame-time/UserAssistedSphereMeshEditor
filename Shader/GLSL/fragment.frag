#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;  // Now this is the direction of light

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 ViewDir;

out vec4 FragColor;

uniform Material material;
uniform Light light;

const float ALPHA_MIN = 0.2;
const float ALPHA_MAX = 0.8;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient * 1.5;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(ViewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

//    float viewAngleCosine = dot(norm, ViewDir);
//    float clampedCosine = clamp(viewAngleCosine, 0.0, 1.0);
//    float alpha = mix(ALPHA_MAX, ALPHA_MIN, clampedCosine);

    float D = dot(norm, ViewDir);
    float t = 1 - (D * D);
    float alpha = mix(ALPHA_MIN, ALPHA_MAX, t);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, alpha);
}

#version 430
in vec3 worldPos;
in vec3 normal;

out vec4 fragColor;

uniform vec3 u_cameraPos;
uniform vec3 u_objectColor;
uniform vec3 u_lightColor;
uniform vec3 u_lightPos;
uniform int u_shininess;

const float AMBIENT_STRENGTH = 0.1;
const float SPECULAR_STRENGTH = 0.5;

void main()
{
    vec3 norm = normalize(normal);

    // Ambient light
    vec3 ambient = AMBIENT_STRENGTH * u_lightColor;

    // Diffuse light
    vec3 lightDir = normalize(u_lightPos - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;

    // Specular light
    vec3 cameraDir = normalize(u_cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(cameraDir, reflectDir), 0.0), u_shininess);
    vec3 specular = SPECULAR_STRENGTH * spec * u_lightColor;  

    vec3 result = (ambient + diffuse + specular) * u_objectColor;
    fragColor = vec4(result, 1.0);
}
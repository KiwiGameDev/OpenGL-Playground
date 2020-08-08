#version 430
in vec3 worldPos;
in vec2 texCoords;
in vec3 normal;

out vec4 fragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material u_material;
uniform Light u_light;

uniform vec3 u_cameraPos;
uniform vec3 u_objectColor;

void main()
{
    // Common light variables
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(u_light.position - worldPos);

    // Ambient light
    vec3 ambient = vec3(texture(u_material.diffuse, texCoords)) * u_light.ambient;

    // Diffuse light
    float diffuseAngle = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(texture(u_material.diffuse, texCoords)) * u_light.diffuse * diffuseAngle;

    // Specular light
    vec3 cameraDir = normalize(u_cameraPos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specAngle = max(dot(cameraDir, reflectDir), 0.0);
    float spec = pow(specAngle, u_material.shininess);
    vec3 specular = vec3(texture(u_material.specular, texCoords)) * u_light.specular * spec;  

    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}
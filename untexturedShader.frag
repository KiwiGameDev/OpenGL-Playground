#version 430
in vec3 WorldPos;
in vec3 Normal;

out vec4 fragColor;

struct Material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 direction;
    float cutoff;
    float outerCutoff;

    float constant;
    float linear;
    float quadratic;
};

#define NR_POINT_LIGHTS 2

uniform Material u_material;
uniform DirLight u_dirLight;
uniform PointLight u_pointLights[NR_POINT_LIGHTS];
uniform SpotLight u_spotLight;

uniform vec3 u_viewPos;

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir);

void main()
{
    // Properties
    vec3 result = vec3(0.0);
    vec3 norm = Normal;
    vec3 viewDir = normalize(u_viewPos - WorldPos);

    // Directional lighting
    result += CalcDirLight(u_dirLight, norm, viewDir);

    // Point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(u_pointLights[i], norm, WorldPos, viewDir);    

    // Spot light
    result += CalcSpotLight(u_spotLight, norm, WorldPos, viewDir);

    fragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // Ambient shading
    vec3 ambient = light.ambient * u_material.diffuse;

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * u_material.diffuse;

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(specAngle, u_material.shininess);
    vec3 specular = light.specular * spec * u_material.specular;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Ambient shading
    vec3 ambient = light.ambient * u_material.diffuse;

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * u_material.diffuse;

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);
    vec3 specular = light.specular * spec * u_material.specular;

    // Attenuation
    float dist   = length(light.position - fragPos);
    float denom1 = light.constant;
    float denom2 = light.linear * dist;
    float denom3 = light.quadratic * dist * dist;
    float attenuation = 1.0 / (denom1 + denom2 + denom3);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    // Spot Light
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse light
    float diffuseAngle = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = u_material.diffuse * light.diffuse * diffuseAngle;

    // Specular light
    vec3 reflectDir = reflect(-lightDir, norm);
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(specAngle, u_material.shininess);
    vec3 specular = u_material.specular * light.specular * spec;  

    // Attenuation
    float dist   = length(light.position - fragPos);
    float denom1 = light.constant;
    float denom2 = light.linear * dist;
    float denom3 = light.quadratic * dist * dist;
    float attenuation = 1.0 / (denom1 + denom2 + denom3);

    // Intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);   

    return (diffuse + specular) * attenuation * intensity;
}
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

//struct Material
//{
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//    float shininess;
//};

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light
{
    vec3 position;      // Not used in directional lights
    vec3 direction;     // Used in directional lights
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;     // For point lights
    float linear;       // For point lights
    float quadratic;    // For point lights
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    // Calculate the Attenuation
    float distance = length(light.position - Position);
    float attenuation = 1.0f / light.constant + light.linear * distance + light.quadratic * (distance * distance);
    
    // Calculate Ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    ambient *= attenuation;
    //vec3 ambient = light.ambient * material.diffuse;
    
    // Calculate Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    //vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    diffuse *= attenuation;
    //vec3 diffuse = light.diffuse * diff * material.diffuse;
    
    // Calculate Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    specular *= attenuation;
    //vec3 specular = light.specular * spec * (vec3(1.0f) - vec3(texture(material.specular, TexCoords)));
    //vec3 specular = light.specular * spec * material.specular;
    
    // Calculate Emission
    vec3 emission = vec3(texture(material.emission, TexCoords));
    
    // Result
    vec3 result = ambient + diffuse + specular + emission;
    color = vec4(result, 1.0f);
}
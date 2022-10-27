#version 430 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};

struct Light {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

uniform vec3 highlightPos1;
uniform vec3 highlightPos2;
uniform vec3 highlightPos3;

uniform bool hasTexture;
uniform vec3 modelColor;

void main()
{
    if (light.position.w == 0.0) {  // directional light
        // ambient
        vec3 ambient;
        if (hasTexture) {
            ambient = texture(material.diffuse1, TexCoords).rgb * light.ambient;
        } else {
            ambient = modelColor * light.ambient;
        }

        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position.xyz);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse;
        if (hasTexture) {
            diffuse = texture(material.diffuse1, TexCoords).rgb * diff * light.diffuse;
        } else {
            diffuse = modelColor * diff * light.diffuse;
        }

        // 镜面光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular;
        if (hasTexture) {
            specular = texture(material.specular1, TexCoords).rgb * spec * light.specular;
        } else {
            specular = modelColor * spec * light.specular;
        }

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
    else
    {
        float distance = length(light.position.xyz - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        // 环境光
        vec3 ambient;
        if (hasTexture) {
            ambient = texture(material.diffuse1, TexCoords).rgb * light.ambient;
        } else {
            ambient = modelColor * light.ambient;
        }

        // 漫反射
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position.xyz - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse;
        if (hasTexture) {
            diffuse = texture(material.diffuse1, TexCoords).rgb * diff * light.diffuse;
        } else {
            diffuse = modelColor * diff * light.diffuse;
        }

        // 镜面光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular;
        if (hasTexture) {
            specular = texture(material.specular1, TexCoords).rgb * spec * light.specular;
        } else {
            specular = modelColor * spec * light.specular;
        }

        vec3 result = (ambient + diffuse + specular) * attenuation;

        FragColor = vec4(result, 1.0);
    }

}
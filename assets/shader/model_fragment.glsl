#version 430 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    float shininess;
};

struct Light {
    int type;  // 1: directional, 2: point, 3: spot, 0: none
    vec3 position;
    vec3 direction;
    vec3 color;

    // 系数
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // 衰减
    float constant;
    float linear;
    float quadratic;

    // 聚光裁剪
    float cutOff;
    float outerCutOff;
};
#define MAX_LIGHTS 10
#define DIR_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3
#define TORCH_LIGHT 4


in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[MAX_LIGHTS];
uniform bool hasTexture;
uniform vec3 modelColor;
uniform samplerCube shadowMap;
uniform float far_plane;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor);
float ShadowCalculation(Light light);

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
    vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
    vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
    vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
    vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

void main() {
    vec3 diffuseOriColor = vec3(1.0f);
    vec3 specularOriColor = vec3(1.0f);
    if (hasTexture) {
        diffuseOriColor = texture(material.diffuse1, TexCoords).rgb;
        specularOriColor = texture(material.specular1, TexCoords).rgb;
    }
    else {
        diffuseOriColor = modelColor;
        specularOriColor = vec3(0.5f, 0.5f, 0.5f);
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0f);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].type == DIR_LIGHT) {
            result += CalcDirLight(lights[i], norm, viewDir, diffuseOriColor, specularOriColor);
        }
        else if (lights[i].type == POINT_LIGHT) {
            result += CalcPointLight(lights[i], norm, FragPos, viewDir, diffuseOriColor, specularOriColor);
        }
        else if (lights[i].type == SPOT_LIGHT || lights[i].type == TORCH_LIGHT) {
            result += CalcSpotLight(lights[i], norm, FragPos, viewDir, diffuseOriColor, specularOriColor);
        }
    }

    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * diffuseColor;
    vec3 diffuse  = light.diffuse  * light.color * diff * diffuseColor;
    vec3 specular = light.specular * spec * specularColor;
    // 阴影
    float shadow = ShadowCalculation(light);
    return ambient + (1.0 - min(shadow, 0.75)) * (diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // 合并结果
    vec3 ambient  = light.ambient  * diffuseColor;
    vec3 diffuse  = light.diffuse  * light.color * diff * diffuseColor;
    vec3 specular = light.specular * light.color * spec * specularColor;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    // 阴影
    float shadow = ShadowCalculation(light);
    return ambient + (1.0 - min(shadow, 0.75)) * (diffuse + specular);
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffuseColor, vec3 specularColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // 聚光
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // 合并结果
    vec3 ambient  = light.ambient  * diffuseColor;
    vec3 diffuse  = light.diffuse  * light.color * diff * diffuseColor;
    vec3 specular = light.specular * light.color * spec * specularColor;
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    // 阴影
    float shadow = ShadowCalculation(light);
    return ambient + (1.0 - min(shadow, 0.75)) * (diffuse + specular);
}

float ShadowCalculation(Light light)
{
    if (light != lights[0])
        return 0;
    vec3 fragToLight = FragPos - light.position;
    float currentDepth = length(fragToLight);

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float bias = max(0.00020 * (1.0 - dot(normal, lightDir)), 0.00001);

    float shadow = 0.0;
    int samples = 20;
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
        shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}
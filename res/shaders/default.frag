#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

// === СТРУКТУРЫ ===

struct Light {
    int type;           // 0 = point, 1 = directional, 2 = spotlight
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 8

// === UNIFORMS ===

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

uniform vec3 objectColor;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;
uniform vec3 camPos;

// === ФУНКЦИИ ===

// PCF для мягких теней
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 1.0; // Вне shadow map = освещено
    }
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    // Динамический bias в зависимости от угла
    float bias = max(0.0025 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // PCF сглаживание 3x3
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    
    shadow /= 9.0;
    
    // Сохраняем амбиент даже в полной тени
    return shadow * 0.7 + 0.3;  // 30% света в тени, 70% тень
}

// Расчет точечного света
vec3 CalculatePointLight(Light light, vec3 norm, vec3 viewDir, vec3 fragColor) {
    vec3 lightDir = normalize(light.position - FragPos);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * fragColor;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * light.color * matSpecular;
    
    // Затухание с расстоянием
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    
    return (diffuse + specular) * light.intensity * attenuation;
}

// Расчет направленного света
vec3 CalculateDirectionalLight(Light light, vec3 norm, vec3 viewDir, vec3 fragColor) {
    vec3 lightDir = normalize(-light.direction);
    
    // Shadow для направленного света
    float shadowFactor = ShadowCalculation(FragPosLightSpace, norm, lightDir);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * fragColor * shadowFactor;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * light.color * matSpecular * shadowFactor;
    
    return (diffuse + specular) * light.intensity;
}

// Расчет спот-лайта
vec3 CalculateSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 fragColor) {
    vec3 lightDir = normalize(light.position - FragPos);
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * fragColor;
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * light.color * matSpecular;
    
    // Spotlight effect
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Затухание
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);
    
    return (diffuse + specular) * light.intensity * intensity * attenuation;
}

// === MAIN ===

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camPos - FragPos);
    
    // Базовый амбиент свет
    vec3 result = matAmbient * objectColor * 0.4;
    
    // Суммируем все источники света
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        if (lights[i].type == 1) {
            // Направленный свет (с тенями)
            result += CalculateDirectionalLight(lights[i], norm, viewDir, objectColor);
        }
        else if (lights[i].type == 0) {
            // Точечный свет
            result += CalculatePointLight(lights[i], norm, viewDir, objectColor);
        }
        else if (lights[i].type == 2) {
            // Спот-лайт
            result += CalculateSpotLight(lights[i], norm, viewDir, objectColor);
        }
    }
    
    // Гамма-коррекция
    result = pow(result, vec3(1.0 / 2.2));
    
    FragColor = vec4(result, 1.0);
}

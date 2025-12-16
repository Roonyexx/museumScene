#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

// === СТРУКТУРЫ ===
struct Light {
    int type;            // 0 = point, 1 = directional, 2 = spotlight
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float range;
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 8
#define MAX_POINT_SHADOWS 5

// === UNIFORMS ===
uniform Light lights[MAX_LIGHTS];
uniform int numLights;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform samplerCube pointShadowMaps[MAX_POINT_SHADOWS];  // Массив cubemap
uniform int numPointShadows;
uniform float far_plane;

uniform vec3 objectColor;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;
uniform vec3 camPos;

// Текстуры
uniform sampler2D diffuseTexture;
uniform bool useTexture;

// === ФУНКЦИИ ===

// PCF для мягких теней (направленный свет)
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.003 * (1.0 - dot(normal, lightDir)), 0.0008);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;

    return shadow;
}

// Тени для точечного света с индексом cubemap
float PointShadowCalculation(vec3 fragPos, vec3 lightPos, vec3 normal, int shadowMapIndex) {
    if (shadowMapIndex < 0 || shadowMapIndex >= numPointShadows) {
        return 0.0; // Нет shadow map для этого источника
    }
    
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(0.1 * (1.0 - dot(normal, lightDir)), 0.03);

    float shadow = 0.0;
    vec3 sampleOffsetDirections[20] = vec3[](
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float diskRadius = (1.0 + (currentDepth / far_plane)) / 100.0;
    
    for(int i = 0; i < 20; ++i) {
        float closestDepth = texture(pointShadowMaps[shadowMapIndex], 
                                    fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= 20.0;

    return shadow;
}

// Расчет точечного света
vec3 CalculatePointLight(Light light, vec3 norm, vec3 viewDir, vec3 baseColor, int pointLightIndex) {
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * baseColor;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * light.color * matSpecular;

    // Затухание
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    // Тени (используем соответствующую cubemap)
    float shadow = PointShadowCalculation(FragPos, light.position, norm, pointLightIndex);
    float shadowFactor = 1.0 - shadow * 0.8; // 80% затемнения максимум

    return (diffuse + specular) * light.intensity * attenuation * shadowFactor;
}

// Расчет направленного света
vec3 CalculateDirectionalLight(Light light, vec3 norm, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(-light.direction);

    // Тени
    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);
    float shadowFactor = 1.0 - shadow * 0.8;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * baseColor;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matShininess);
    vec3 specular = spec * light.color * matSpecular;

    return (diffuse + specular) * light.intensity * shadowFactor;
}

// Расчет спот-лайта
vec3 CalculateSpotLight(Light light, vec3 norm, vec3 viewDir, vec3 baseColor) {
    vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * matDiffuse * baseColor;

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
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    return (diffuse + specular) * light.intensity * intensity * attenuation;
}

// === MAIN ===
void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camPos - FragPos);

    // Получаем базовый цвет из текстуры или uniform
    vec3 baseColor = objectColor;
    if (useTexture) {
        vec4 texColor = texture(diffuseTexture, TexCoords);
        baseColor = texColor.rgb;
    }

    // Базовый ambient
    vec3 result = matAmbient * baseColor * 0.3;

    // Счетчик точечных источников для индексации shadow maps
    int pointLightIndex = 0;
    
    // Суммируем все источники света
    for (int i = 0; i < numLights && i < MAX_LIGHTS; ++i) {
        if (lights[i].type == 1) {
            // Направленный свет
            result += CalculateDirectionalLight(lights[i], norm, viewDir, baseColor);
        }
        else if (lights[i].type == 0) {
            // Точечный свет (с соответствующей shadow map)
            result += CalculatePointLight(lights[i], norm, viewDir, baseColor, pointLightIndex);
            pointLightIndex++;
        }
        else if (lights[i].type == 2) {
            // Спот-лайт
            result += CalculateSpotLight(lights[i], norm, viewDir, baseColor);
        }
    }

    // Гамма-коррекция
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}

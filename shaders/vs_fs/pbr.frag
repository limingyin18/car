#version 460 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in mat3 TBN;

layout(std140, binding = 0) uniform Camera
{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

layout(std140, binding = 1) uniform Enviroment
{
    vec3 lightColor;
    vec3 lightDir;
};

layout(binding = 0) uniform sampler2D albedoMap;
layout(binding = 1) uniform sampler2D normalMap;
layout(binding = 2) uniform sampler2D ormMap;
layout(binding = 3) uniform samplerCube irradianceMap;
layout(binding = 4) uniform samplerCube prefilterMap;
layout(binding = 5) uniform sampler2D brdfLUT;

vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

const float PI = 3.14159265359;

vec3 getNormalFromNormalMap();

void main()
{
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2f));
    vec3 normal = getNormalFromNormalMap();
    float ao = texture(ormMap, TexCoords).r;
    float roughness = texture(ormMap, TexCoords).g;
    float metallic = texture(ormMap, TexCoords).b;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - WorldPos);
    vec3 R = reflect(-V, N);
    vec3 L = normalize(lightDir);
    vec3 H = normalize(V + L);

    vec3 Lo = vec3(0.0);
    vec3 radiance = lightColor;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    // indirect
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    kS = F;
    kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    // vec3 irradiance = vec3(0.2, 0.2f, 0.2f);
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    specular = prefilteredColor * (F * brdf.x + brdf.y);
    // specular = vec3(0.f);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;
    // vec3 color = Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 getNormalFromNormalMap()
{
    vec3 norm = texture(normalMap, TexCoords).rgb;
    norm = norm * 2.0 - 1.0;
    norm = normalize(TBN * norm);
    return norm;
}
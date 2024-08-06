#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D ssao;

uniform sampler2D uGDepth;

uniform vec3 lightColor;
uniform vec3 lightDir;

uniform mat4 proj;
uniform mat4 invView;
uniform mat4 view;

in mat4 vWorldToScreen;
in vec4 vPosWorld;

float Rand1(inout float p)
{
    p = fract(p* .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

vec2 Rand2(inout float p)
{
    return vec2(Rand1(p), Rand1(p));
}

float InitRand(vec2 uv)
{
    vec3 p3 = fract(vec3(uv.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 SampleHemisphereCos(inout float s, out float pdf)
{
    vec2 uv = Rand2(s);
    float z = sqrt(1.0 - uv.x);
    float phi = uv.y * 2.0 * 3.14159265359;
    float sinTheta = sqrt(uv.x);
    vec3 dir = vec3(sinTheta * cos(phi), sinTheta * sin(phi), z);
    pdf = z * 0.31830988618;
    return dir;
}

void LocalBasis(vec3 n, out vec3 b1, out vec3 b2)
{
    float sign_ = sign(n.z);
    if(n.z == 0.0)
    {
        sign_ = 1.0;
    }
    float a = -1.0 / (sign_ + n.z);
    float b = n.x * n.y * a;
    b1 = vec3(1.0 + sign_ * n.x * n.x * a, sign_ * b, -sign_ * n.x);
    b2 = vec3(b, sign_ + n.y * n.y * a, -n.y);
}

vec4 Project(vec4 a)
{
    return a/a.w;
}

float  GetDepth(vec3 posWorld)
{
    float depth = (vWorldToScreen * vec4(posWorld, 1.0)).w;
    return depth;
}

float GetBufferDepth(vec2 uv)
{
    float depth = texture(uGDepth, uv).x;
    if(depth < 1e-2)
    {
        depth = 1000.0;
    }
    return depth;
}

vec3 GetGBufferNormalWorld(vec2 uv)
{
    vec3 normal = mat3(invView)*texture(gNormal, uv).xyz;
    return normal;
}

vec3 GetGBufferDiffuse(vec2 uv)
{
    vec3 diffuse = texture(gAlbedo, uv).xyz;
    return diffuse;
}

vec3 EvalDiffuse(vec3 wi, vec3 wo, vec2 uv) 
{
    vec3 albedo  = GetGBufferDiffuse(uv);
    vec3 normal = GetGBufferNormalWorld(uv);
    float cos_test = max(0., dot(normal, wi));
    // return albedo * cos;
    return albedo * cos_test;
}

bool RayMarch(vec3 FragPos, vec3 reflectDir, out vec3 hitPos, out vec2 uv)
{
    int stepNum = 150;
    float step1 = 0.05f;
    for(int i = 0; i < stepNum; i++)
    {
        float delta = i * step1;
        vec3 sample1 = FragPos + reflectDir * delta;
        // float sampleDepth = -sample1.z;
        vec4 sample1_view = view*vec4(sample1, 1.0);
        float sampleDepth = -sample1_view.z/sample1_view.w;

        vec4 tmp = proj * view*vec4(sample1, 1.0);
        vec3 projCoords = tmp.xyz / tmp.w;
        uv = projCoords.xy * 0.5 + 0.5;

        float sceneDepth = -texture(gPosition, uv).z;
        if(sampleDepth - sceneDepth > 0.0001)
        {
            hitPos = sample1;
            return true;
            // Diffuse = texture(gAlbedo, uv).rgb;
        }
    }

    return false;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;

    vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)

    vec3 normal = mat3(invView)*Normal;
    vec4 worldPos_4 = (invView)*vec4(FragPos, 1.0f);
    vec3 worldPos = worldPos_4.xyz / worldPos_4.w;
    float s = InitRand(gl_FragCoord.xy);
    vec3 b1, b2;
    LocalBasis(normal, b1, b2);
    vec3 wi = normalize(mat3(invView)*lightDir);
    vec3 wo = normalize(mat3(invView)*viewDir);

    vec3 L_ind = vec3(0.0);
    bool is_hit = false;

    vec3 dir = vec3(0.0);
    for(int i = 0; i < 4; i++)
    {
        float pdf; 
        vec3 localDir = SampleHemisphereCos(s, pdf);
        dir = normalize(mat3(b1, b2, normal) * localDir);
        vec3 position_1;
        vec2 uv;
        if(RayMarch(worldPos, dir, position_1, uv))
        {
            L_ind += EvalDiffuse(dir, wo, TexCoords) / pdf * EvalDiffuse(wi, dir, uv);
            // L_ind += EvalDiffuse(wi, dir, uv);
            is_hit = true;
            // vec3 albedo = texture(gAlbedo, uv).xyz;
            // vec3 normal_test = GetGBufferNormalWorld(uv);
            // float cos_test = max(0., dot(normal, wi));
            // vec3 test_color =  albedo * cos_test;
            // L_ind += EvalDiffuse(wi, dir, uv);
            // L_ind = wi;
            // break;
        }
    }

    L_ind /= float(4);
    // L_ind = normal;

    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;

    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // then calculate lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    // vec3 ambient = vec3(0.3 * Diffuse );
    vec3 lighting  = ambient; 
    // diffuse
    // vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 256.0);
    vec3 specular = lightColor * spec * 0.5;
    lighting += diffuse + specular;

    // if(is_hit)
    // {
    //     FragColor = vec4(1.0);
    // }
    // else
    // {
    //     FragColor = vec4(0.f, 0.f, 0.f, 1.0f);
    // }
    // FragColor = vec4(is_hit, 1.0);

    vec3 L = lighting+L_ind;
    // vec3 L = L_ind;
    if(is_hit)
    {
        L /= 2.0;
    }

    // vec3 L = L_ind;
    // vec3 color = pow(clamp(L, vec3(0.0), vec3(1.0)), vec3(1.0 / 2.2));
    FragColor = vec4(L, 1.0);
}
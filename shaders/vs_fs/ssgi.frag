#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D gPosition;
layout(binding = 1) uniform sampler2D gNormal;
layout(binding = 2) uniform sampler2D gAlbedo;
layout(binding = 3) uniform sampler2D ssao;

layout(binding = 4) uniform sampler2D uGDepth;

uniform vec3 lightColor;
uniform vec3 lightDir;

uniform mat4 proj;
uniform mat4 invView;
uniform mat4 view;

uniform int maxLevel;

#define MAX_THICKNESS 0.0017
#define INV_PI 0.31830988618



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

vec3 GetScreenCoord(vec3 Point)
{
   	vec4 positionInScreen = proj*vec4(Point,1.0);
   	// vec4 positionInScreen = vec4(Point,1.0);
	positionInScreen.xyzw /= positionInScreen.w;
	positionInScreen.xyz = positionInScreen.xyz*0.5+0.5;
    return positionInScreen.xyz; 
}

vec2 getCellCount(int level){
    return textureSize(uGDepth,level);
}

vec2 getCell(vec2 pos,vec2 startCellCount){

 return vec2(floor(pos*startCellCount));
}

vec3 intersectDepthPlane(vec3 o, vec3 d, float t){
    return o + d * t;
}

vec3 intersectCellBoundary(vec3 o,vec3  d, vec2 rayCell,vec2 cell_count, vec2 crossStep, vec2 crossOffset){

    vec2 nextPos = rayCell + crossStep ;
    nextPos = nextPos/cell_count;
    nextPos = nextPos+crossOffset;

    vec2 dis  = nextPos - o.xy;

    vec2 delta = dis/d.xy;

    float t = min(delta.x,delta.y);

    return intersectDepthPlane(o,d,t);
}

float getMinimumDepthPlane(vec2 pos , int level){

    return textureLod(uGDepth,pos,level).r;
}

bool crossedCellBoundary(vec2 oldCellIdx,vec2 newCellIdx){
    return (oldCellIdx.x!=newCellIdx.x)||(oldCellIdx.y!=newCellIdx.y);
}

bool FindIntersection(vec3 start,vec3 rayDir,float maxTraceDistance, out vec3 hitPos){

    vec2 crossStep = vec2(rayDir.x>=0?1:-1,rayDir.y>=0?1:-1);
    vec2 crossOffset = crossStep / vec2(1280.0,960.0) / 128;
    crossStep = clamp(crossStep,0.0,1.0);

    vec3 ray = start;
    float minZ = ray.z;
    float maxZ = ray.z+rayDir.z*maxTraceDistance;
    float deltaZ = (maxZ-minZ);

    vec3 o = ray;
    vec3 d = rayDir*maxTraceDistance;

    int startLevel = 0;
    int stopLevel = 0;
    vec2 startCellCount = getCellCount(startLevel);


    vec2 rayCell = getCell(ray.xy,startCellCount);
    ray = intersectCellBoundary(o, d, rayCell, startCellCount, crossStep, crossOffset*64);

    int level = startLevel;
    int iter = 0;
    bool isBackwardRay = rayDir.z<0;
    
    float Dir = isBackwardRay ? -1 : 1;

    while( level>=stopLevel && ray.z*Dir <= maxZ*Dir && iter<100){
        vec2 cellCount = getCellCount(level);
        vec2 oldCellIdx = getCell(ray.xy,cellCount);

        float cell_minZ = getMinimumDepthPlane(ray.xy, level);

        vec3 tmpRay = ((cell_minZ>ray.z) && !isBackwardRay) ? intersectDepthPlane(o,d,(cell_minZ-minZ)/deltaZ) :ray;

        vec2 newCellIdx = getCell(tmpRay.xy,cellCount);

        float thickness = level ==0 ? (ray.z-cell_minZ) : 0;
        bool crossed  = (isBackwardRay&&(cell_minZ>ray.z))||(thickness>MAX_THICKNESS)|| crossedCellBoundary(oldCellIdx, newCellIdx);
        hitPos =  vec3(rayDir.z);

        ray = crossed ? intersectCellBoundary(o, d, oldCellIdx, cellCount, crossStep, crossOffset):tmpRay;
        level = crossed ? min(maxLevel,level+1):level-1;
        ++iter;
    }
    bool intersected = (level < stopLevel);
    // hitPos = vec3(maxZ*Dir);
	
    // return intersected ? texture(gAlbedo,hitPos.xy).rgb:vec3(0.0);
    return intersected;
    // return intersected ? vec3(1.0):vec3(0.0);
    // return intersected ? vec3(level):vec3(0.0);
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

bool RayMarch_Hiz(vec3 ori, vec3 dir, out vec3 hitPos, out vec2 uv){
    float step = 0.005;
    float maxDistance = 7.5;

    int startLevel = 2;
    int stopLevel = 0;

    vec3 curPos = ori;
    int level = startLevel;
    while(level >= stopLevel && distance(ori, curPos) < maxDistance){
        // float rayDepth = GetDepth(curPos);

        float a = -1.0202;
        float b = -0.202;

        // vec4 sample1_cs =view*vec4(curPos, 1.0);
        vec4 sample1_cs = proj * view*vec4(curPos, 1.0);
        sample1_cs /= sample1_cs.w;
        float rayDepth = (sample1_cs.z * 0.5 + 0.5);
        // float rayDepth = sample1_cs.z;
        // rayDepth = a * rayDepth + b;
        // rayDepth /= -sample1_cs.z;
        // rayDepth *= 0.5;
        // rayDepth += 0.5;

        vec4 tmp = proj * view*vec4(curPos, 1.0);
        tmp /= tmp.w;
        uv = tmp.xy * 0.5 + 0.5;
        if(uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        {
            break;
        }
        // vec2 screenUV = GetScreenCoordinate(curPos);
        // float gBufferDepth = -texture(gPosition, uv).z;
        // vec3 gBufferPos = texture(gPosition, uv).rgb;
        // vec4 sample1_cs_g =vec4(gBufferPos, 1.0);
        // sample1_cs_g /= sample1_cs_g.w;
        // float gBufferDepth = (sample1_cs_g.z);
        // gBufferDepth = a * gBufferDepth + b;
        // // gBufferDepth /= sample1_cs_g.z;
        // gBufferDepth /= -sample1_cs_g.z;


        float gBufferDepth = getMinimumDepthPlane(uv, level);
        // hitPos = vec3(gBufferDepth, rayDepth, 0.0);
        // return true;

        // if(rayDepth - gBufferDepth < -0.000001)
        if(rayDepth - gBufferDepth > 0.000001)
        {
          if(level == 0){
            hitPos = curPos;
        // hitPos = vec3(gBufferDepth, rayDepth, 0.0);
        hitPos = sample1_cs.xyz;
            return true;
          }
          else{
            level = level - 1;
        // hitPos = vec3(gBufferDepth, rayDepth, level);
        //     return true;
          }
        }
        else{
          level = min(maxLevel, level + 1);
        //   level = 0;
          vec3 stepDistance = (dir * step * float(level + 1));
          curPos += stepDistance;
        //   if(level == 8)
          {
                    vec4 sample1_cs =view*vec4(curPos, 1.0);
                    // vec4 sample1_cs = proj * view*vec4(curPos, 1.0);
                    sample1_cs /= sample1_cs.w;
                    if(-sample1_cs.z < 0.1)
                    {
                        return false;
                    }
                    // // float rayDepth = -(sample1_cs.z * 0.5 + 0.5);
                    // float rayDepth = sample1_cs.z;
                    // rayDepth = a * rayDepth + b;
                    // rayDepth /= -sample1_cs.z;
                    // hitPos = vec3(gBufferDepth, sample1_cs.z, level);
                    // return true;
          }
        //   hitPos = stepDistance;
        //   return true;
        }
    }

    hitPos = vec3(0);
    return false;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 positionInView = texture(gPosition,TexCoords).rgb;
    vec3 normalInView = texture(gNormal,TexCoords).rgb;
    vec3 relfectDir = normalize(reflect(normalize(positionInView),normalInView));
    vec3 Normal = texture(gNormal, TexCoords).rgb;


    vec3 endPosInView = positionInView+relfectDir*1000;
    // endPosInView /= (endPosInView.z < 0 ? endPosInView.z : 1);

    vec3 start = GetScreenCoord(positionInView);
    vec3 end = GetScreenCoord(endPosInView);
    vec3 rayDir = normalize(end-start);

    float maxTraceX = rayDir.x>=0 ? (1-start.x)/rayDir.x:-start.x/rayDir.x;
    float maxTraceY = rayDir.y>=0 ? (1-start.y)/rayDir.y:-start.y/rayDir.y;
    float maxTraceZ = rayDir.z>=0 ? (1-start.z)/rayDir.z:-start.z/rayDir.z;
    float maxTraceDistance = min(maxTraceX,min(maxTraceY,maxTraceZ));

    vec3 L_ind = vec3(0.0);
    bool is_hit = false;
    vec3 hitPos = vec3(0.0);

    vec3 viewDir  = normalize(-positionInView); // viewpos is (0.0.0)
    vec3 wi = normalize(mat3(invView)*lightDir);
    vec3 wo = normalize(mat3(invView)*viewDir);
    for(int i = 0; i < 1; i++)
    {
        if(FindIntersection(start, rayDir, maxTraceDistance, hitPos))
        {
            FragColor = vec4(relfectDir, 1.0);
            return;
            L_ind += EvalDiffuse(rayDir, wo, TexCoords) * EvalDiffuse(wi, rayDir, hitPos.xy);
            is_hit = true;
        }
        else
        {
            FragColor = vec4(relfectDir, 0.5);
            return;
        }
    }

    L_ind /= float(1);

    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;

    float AmbientOcclusion = texture(ssao, TexCoords).r;
    AmbientOcclusion = 1.0f;
    // then calculate lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    // vec3 ambient = vec3(0.3 * Diffuse );
    vec3 lighting  = ambient; 
    // diffuse
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 256.0);
    vec3 specular = lightColor * spec * 0.5;
    lighting += diffuse + specular;

    vec3 L = lighting + L_ind;
    // if(is_hit)
    // {
    //     L /= 2.0;
    // }

    FragColor = vec4(L, 1.0);
    // FragColor = vec4(vec3(is_hit), 1.0);
}

// void main()
// {             
//     // retrieve data from gbuffer
//     vec3 FragPos = texture(gPosition, TexCoords).rgb;
//     vec3 Normal = texture(gNormal, TexCoords).rgb;

//     vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)

//     vec3 normal = mat3(invView)*Normal;
//     vec4 worldPos_4 = (invView)*vec4(FragPos, 1.0f);
//     vec3 worldPos = worldPos_4.xyz / worldPos_4.w;
//     float s = InitRand(gl_FragCoord.xy);
//     vec3 b1, b2;
//     LocalBasis(normal, b1, b2);
//     vec3 wi = normalize(mat3(invView)*lightDir);
//     vec3 wo = normalize(mat3(invView)*viewDir);

//     vec3 L_ind = vec3(0.0);
//     bool is_hit = false;

//     vec3 dir = vec3(0.0);
//     for(int i = 0; i < 1; i++)
//     {
//         float pdf; 
//         vec3 localDir = SampleHemisphereCos(s, pdf);
//         dir = normalize(mat3(b1, b2, normal) * localDir);
//         dir = normalize(reflect(-wo, normal));
//         vec3 position_1;
//         vec2 uv;
//         if(RayMarch_Hiz(worldPos, dir, position_1, uv))
//         {
//             // FragColor = vec4(position_1, 1.0);
//             // return;
//             L_ind += EvalDiffuse(dir, wo, TexCoords) * EvalDiffuse(wi, dir, uv);
//             is_hit = true;
//         }
//     }

//     L_ind /= float(1);

//     vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;

//     float AmbientOcclusion = texture(ssao, TexCoords).r;
//     AmbientOcclusion = 1.0f;
//     // then calculate lighting as usual
//     vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
//     // vec3 ambient = vec3(0.3 * Diffuse );
//     vec3 lighting  = ambient; 
//     // diffuse
//     vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lightColor;
//     // specular
//     vec3 halfwayDir = normalize(lightDir + viewDir);  
//     float spec = pow(max(dot(Normal, halfwayDir), 0.0), 256.0);
//     vec3 specular = lightColor * spec * 0.5;
//     lighting += diffuse + specular;

//     vec3 L = lighting + L_ind;
//     // if(is_hit)
//     // {
//     //     L /= 2.0;
//     // }

//     FragColor = vec4(L, 1.0);
//     // FragColor = vec4(vec3(is_hit), 1.0);
// }
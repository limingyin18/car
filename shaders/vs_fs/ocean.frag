#version 460 core

layout (location = 0) in vec3 normalFrag;
layout (location = 1) in vec3 posFrag;

layout (location = 0) out vec4 colorFragOut;

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

void main()
{
    float Kdiffuse = 0.91f;

    float reflectivity;
    float nSnell = 1.34f;
    float costhetai = abs(dot(lightDir, normalFrag));
    float thetai = acos(costhetai);
    float sinthetat = sin(thetai)/nSnell;
    float thetat = asin(sinthetat);
    if(thetai == 0.0)
    {
        reflectivity = (nSnell - 1)/(nSnell + 1);
        reflectivity = reflectivity * reflectivity;
    }
    else
    {
        float fs = sin(thetat - thetai) / sin(thetat + thetai);
        float ts = tan(thetat - thetai) / tan(thetat + thetai);
        reflectivity = 0.5 * ( fs*fs + ts*ts );
    }


    float ambient = 0.1f;

    float specular = 0.0f;
    float diffuse = max(dot(normalFrag, lightDir), 0.0);
    if(diffuse > 0.01f)
    {
        vec3 viewDir = normalize(viewPos - posFrag);
        vec3 reflectDir = reflect(-lightDir, normalFrag);
        specular = pow(max(dot(viewDir, reflectDir), 0.0), 256) * 0.5f;
    }

    vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f);
    colorFragOut = vec4((ambientColor + 
        reflectivity * vec3(0.69f, 0.84f, 1.f) + (1-reflectivity) * vec3(0.0f, 0.2f, 0.3f))*lightColor +
        specular * lightColor, 1.0f);

    // colorFragOut = vec4(specular*lightColor, 1.0f);
    
}
cbuffer PointLightCbuf : register(b0)
{
    float3 LightPos;
    float3 light_ambient;
    float3 light_diffuse;
    float attIntensity;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};

cbuffer PointLightCBuf : register(b0)
{
    float3 viewLightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};
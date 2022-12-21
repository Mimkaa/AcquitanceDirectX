cbuffer PointLightCbuf
{
    float3 LightPos;
    float3 light_ambient;
    float3 light_diffuse;
    float attIntensity;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};
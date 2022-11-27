cbuffer LightCBuf
{
    float3 LightPos;
    float3 light_ambient;
    float3 light_diffuse;
    float attIntensity;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};


cbuffer MaterialCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalsMappingOn;
    float padding[1];
};

cbuffer CBufMat
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D norm;
SamplerState smpl;

float4 main(float3 ViewPos : Position, float3 normal_in : Normal, float2 tec : Texcoord) : SV_Target
{
    float3 normal;
    if (normalsMappingOn == 1)
    {
        normal = norm.Sample(smpl, tec).rgb;
        normal.x = normal.x * 2 - 1;
        normal.y = -(normal.y * 2 - 1);
        normal.z = -normal.z;
        normal = mul(normal, (float3x3) modelView);
    }
    else
    {
        normal = normal_in;
    }
    

    const float3 v_to_l = LightPos - ViewPos;
    const float dist = length(v_to_l);
    const float3 LightDir = v_to_l / dist;
    const float attenuation = 1.0f /
                (constant_attenuation + linear_attenuation * dist + quadratic_attenuation * pow(dist, 2));

    const float3 d = light_diffuse * attenuation * attIntensity * max(0.0f, dot(normal, LightDir));

    // specular highlight
    const float3 w = normal * dot(v_to_l, normal);
    // opposite direction of the reflection
const float3 r = w * 2.0f - v_to_l;
//specular intensity between view vector an the reflection
const float3 specular = attenuation * (light_diffuse * attIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(ViewPos))), specularPower);


return float4(saturate((d + light_ambient) * tex.Sample(smpl, tec).rgb + specular), 1.0f);


}
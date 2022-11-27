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
   
    bool normalsMappingOn;
    float padding[3];
};

cbuffer CBufMat
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D spec;
Texture2D norm;
SamplerState smpl;

float4 main(float3 ViewPos : Position, float3 normal_in : Normal, float2 tec : Texcoord, float3 tan : Tangent, float3 btan : Bytangent) : SV_Target
{
    float3 normal;
    if (normalsMappingOn == 1)
    {
        float3x3 TBNmat = float3x3(normalize(tan), normalize(btan), normalize(normal_in));
        float3 normalSample = norm.Sample(smpl, tec).xyz;
        normal.x = normalSample.x * 2 - 1;
        normal.y = -(normalSample.y * 2 - 1);
        normal.z = normalSample.z * 2 - 1;
        
        normal_in = mul(normal_in,TBNmat);
        normal = normal_in;
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
//specular intensity between view vector an the reflection
const float4 specularSample = spec.Sample(smpl, tec);
const float3 specularReflectionColor = specularSample.rgb;
const float  specularPower = pow(2.0f, specularSample.a * 13.0f);
const float3 specular = attenuation * (light_diffuse * attIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(ViewPos))), specularPower);


return float4(saturate((d + light_ambient) * tex.Sample(smpl, tec).rgb + specular), 1.0f);


}
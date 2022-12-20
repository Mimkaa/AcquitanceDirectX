cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};
Texture2D tex;
Texture2D spec;



SamplerState smpl;

float4 main(float3 ViewPos : Position, float3 normal : Normal, float2 tec : Texcoord) : SV_Target
{
    normal = normalize(normal);
    const float3 v_to_l = lightPos - ViewPos;
    const float dist = length(v_to_l);
    const float3 LightDir = v_to_l / dist;
    const float attenuation = 1.0f / (attConst + attLin * dist + attQuad * (dist*dist));

    const float3 d = diffuseColor * attenuation * diffuseIntensity * max(0.0f, dot(LightDir, normal));

    // specular highlight
    const float3 w = normal * dot(v_to_l, normal);
    // opposite direction of the reflection
    const float3 r = w * 2.0f - v_to_l;
    //specular intensity between view vector an the reflection
    const float4 specularSample = spec.Sample(smpl, tec);
    const float3 specularReflectionColor = specularSample.rgb;
    const float  specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = attenuation * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(ViewPos))), specularPower);

    return float4(saturate((d + ambient) * tex.Sample(smpl, tec).rgb + specular * specularReflectionColor), 1.0f);


}
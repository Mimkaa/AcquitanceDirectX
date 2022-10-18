cbuffer LightCBuf
{
    float3 LightPos;
    float3 material_color;
    float3 light_ambient;
    float3 light_diffuse;
    float attIntensity;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
};





float4 main(float3 WorldPos : Position, float3 normal : Normal) : SV_Target
{
    const float3 v_to_l = LightPos - WorldPos;
    const float dist = length(v_to_l);
    const float3 LightDir = v_to_l / dist;
    const float attenuation = 1.0f/
				(constant_attenuation + linear_attenuation * dist + quadratic_attenuation * pow(dist, 2));
    
    const float3 d = light_diffuse * attenuation * attIntensity * max(0.0f, dot(normal, LightDir));
    
    
    return float4(saturate(material_color * (d + light_ambient)), 1.0f);
    
    
}
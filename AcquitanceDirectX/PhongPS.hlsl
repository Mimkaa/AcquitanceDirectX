cbuffer LightCBuf
{
    float3 LightPos;
};


		// color of direct light
static float3 light_diffuse = { 1.0f, 1.0f, 1.0f };
		// indirect light
static float3 light_ambient = { 0.1f, 0.1f, 0.1f };
		// how much of each channel is reflected
static float3 material_color = { 0.8f, 0.85f, 1.0f };

static const float linear_attenuation = 0.045f;
static const float quadratic_attenuation = 0.007f;
static const float constant_attenuation = 1.0f;
static const float attIntensity = 1.0f;


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
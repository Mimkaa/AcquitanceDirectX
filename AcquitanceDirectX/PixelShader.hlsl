cbuffer CBuf
{
	float4 face_color[6];
};
float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return face_color[tid / 2];
}
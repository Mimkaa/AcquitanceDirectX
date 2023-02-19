struct VSOut
{
 
    float2 uv : TexCoord;
    float4 pos : SV_Position;

};

VSOut main(float2 pos : Position)
{
    VSOut outt;
    outt.pos = float4(pos, 0.0f, 1.0f);
    outt.uv = float2((pos.x + 1) / 2.0f, -(pos.y - 1) / 2.0f);
	return outt;
}
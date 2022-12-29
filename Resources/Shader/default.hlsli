struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

// in을 받아줘서 out을 내보냄
VS_OUT VS_Main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	// pos, color를 똑같이 변환해줌
	// 즉 아무것도 안하는거
	output.pos = float4(input.pos, 1.f);
	output.color = input.color;

	return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}
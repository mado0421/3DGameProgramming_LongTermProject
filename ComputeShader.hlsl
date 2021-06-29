#include "Header.hlsli"

[numthreads(256, 1, 1)]
void CS_main( uint3 DTid : SV_DispatchThreadID )
{
	gtxtPostProcessMap[DTid.xy] = float4(gtxtColorMap[DTid.xy].xyz * float3(1.7f, 0.3f, 0.3f), gtxtColorMap[DTid.xy].w);
	//gtxtPostProcessMap[DTid.xy] = float4(1,1,1,1);
}

[numthreads(256, 1, 1)]
void CS_VerticalBlur(uint3 DTid : SV_DispatchThreadID)
{
	gtxtPostProcessMap[DTid.xy] = gtxtColorMap[DTid.xy];
}

[numthreads(1, 256, 1)]
void CS_HorizontalBlur(uint3 DTid : SV_DispatchThreadID)
{
	gtxtPostProcessMap[DTid.xy] = gtxtColorMap[DTid.xy];
}
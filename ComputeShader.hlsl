#include "Header.hlsli"

[numthreads(256, 1, 1)]
void CS_main( uint3 DTid : SV_DispatchThreadID )
{
	gtxtPostProcessMap[DTid.xy] = float4(gtxtColorMap[DTid.xy].xyz * float3(1.7f, 0.3f, 0.3f), gtxtColorMap[DTid.xy].w);
	//gtxtPostProcessMap[DTid.xy] = float4(1,1,1,1);
}

static const int	gMaxBlurRadius = 5;
#define NBlurTh 256
#define CacheSize (NBlurTh + 2 * gMaxBlurRadius)
groupshared float4	gBlurCache[CacheSize];

[numthreads(256, 1, 1)]
void CS_VerticalBlur(
	uint3 GTid : SV_GroupThreadID,
	uint3 DTid : SV_DispatchThreadID)
{
	int					gBlurRadius = 4;

	float weights[9] = {
		0.000133831,
		0.00443186,
		0.0539911,
		0.241971,
		0.398943,
		0.241971,
		0.0539911,
		0.00443186,
		0.000133831
	};

	if (GTid.x < gBlurRadius) {
		int x = max(DTid.x - gBlurRadius, 0);
		gBlurCache[GTid.x] = gtxtColorMap[int2(x, DTid.y)];
	}
	if (GTid.x >= NBlurTh - gBlurRadius) {
		int x = min(DTid.x + gBlurRadius, gtxtColorMap.Length.x - 1);
		gBlurCache[GTid.x + 2 * gBlurRadius] = gtxtColorMap[int2(x, DTid.y)];
	}
	gBlurCache[GTid.x + gBlurRadius] = gtxtColorMap[min(DTid.xy, gtxtColorMap.Length.xy-1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		int k = GTid.x + gBlurRadius + i;
		blurColor += weights[i + gBlurRadius] * gBlurCache[k];
	}

	gtxtPostProcessMap[DTid.xy] = blurColor;
}

[numthreads(1, 256, 1)]
void CS_HorizontalBlur(
	uint3 GTid : SV_GroupThreadID,
	uint3 DTid : SV_DispatchThreadID)
{
	int					gBlurRadius = 4;

	float weights[9] = {
	0.000133831,
	0.00443186,
	0.0539911,
	0.241971,
	0.398943,
	0.241971,
	0.0539911,
	0.00443186,
	0.000133831
	};

	if (GTid.y < gBlurRadius) {
		int y = max(DTid.y - gBlurRadius, 0);
		gBlurCache[GTid.y] = gtxtColorMap[int2(DTid.x, y)];
	}
	if (GTid.y >= NBlurTh - gBlurRadius) {
		int y = min(DTid.y + gBlurRadius, gtxtColorMap.Length.y - 1);
		gBlurCache[GTid.y + 2 * gBlurRadius] = gtxtColorMap[int2(DTid.x, y)];
	}
	gBlurCache[GTid.y + gBlurRadius] = gtxtColorMap[min(DTid.xy, gtxtColorMap.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		int k = GTid.y + gBlurRadius + i;
		blurColor += weights[i + gBlurRadius] * gBlurCache[k];
	}

	gtxtPostProcessMap[DTid.xy] = blurColor;
}
float4x4	World;
float4x4	View;
float4x4	Projection;
float		Scale = 0.25f;
float3		Color = float3(0.f, 0.f, 0.f);

//--------------------------------------------------------------------------------------
// VertexShader I/O
//--------------------------------------------------------------------------------------
struct VSInput {
	float3	Position : POSITION; 
	float3	Normal : NORMAL;
};

struct VSOutput {
	float4	Position : POSITION;
};

VSOutput
VS(
	VSInput	input
)
{
	VSOutput	output;
    
	float3	worldPosition = mul(float4(input.Position, 1.f), World);
	float3	worldNormal = float3(0.f, 0.f, 0.f);
	worldNormal = mul(input.Normal, (float3x3) World);
	worldPosition += worldNormal * Scale;
	output.Position = mul(mul(float4(worldPosition, 1.f), View), Projection);
	output.Position.z -= 1.0e-4;

	return output;    
}

//--------------------------------------------------------------------------------------
// PixelShader I/O
//--------------------------------------------------------------------------------------
struct PSOutput {
	float4	Color : COLOR0;		// Pixel color    
};

PSOutput PS() 
{ 
	PSOutput	output;
    
	output.Color = float4(Color, 1.f);

	return output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique Standard {
	pass P0 {   
		CullMode = None;
		ZWriteEnable = false;

		VertexShader = compile vs_2_0 VS();
		PixelShader  = compile ps_2_0 PS(); 
	}
}

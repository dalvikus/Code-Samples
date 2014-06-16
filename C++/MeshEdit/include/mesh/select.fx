float4x4	mWorldViewProjection;	// World * View * Projection matrix
float		pointSize = 4.f;
float4		pointColor = float4(1.f, 0.f, 1.f, 1.f);
DWORD	g__mode = 0;	// 0:= Point, 1:= Rectangle
float	g__zDepth = 0;	// 격자를 제일 앞에 표시할 지 아니면 제일 뒤에 표시할 지...

//--------------------------------------------------------------------------------------
// VertexShader I/O
//--------------------------------------------------------------------------------------
struct VSInput
{
	float4	Position : POSITION;	// vertex position
};

struct VSOutput
{
	float4	Position : POSITION;
	float	Size : PSIZE;
};

VSOutput VS(VSInput input)
{
	VSOutput output;

if (g__mode == 1 || g__mode == 2) {
	output.Position = input.Position;
	if (g__mode == 2) {
		output.Position.z = g__zDepth;
	}
} else {
	// Transform the position from object space to homogeneous projection space
	output.Position = mul(input.Position, mWorldViewProjection);
	output.Position.z -= 1.e-4;	// Place virtual point in front of real point!

	output.Size = pointSize;
}

	return output;
}

//--------------------------------------------------------------------------------------
// PixelShader I/O
//--------------------------------------------------------------------------------------
struct PSOutput
{
	float4	Color : COLOR0;		// Pixel color
};


PSOutput PS()
{
	PSOutput	output;

	output.Color = pointColor;

	return output;
}


//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique Standard
{
	pass P0 {
		CullMode = None;
		ZWriteEnable = false;
		PointScaleEnable = false;
		VertexShader = compile vs_2_0 VS();
		PixelShader = compile ps_2_0 PS();
	}
}

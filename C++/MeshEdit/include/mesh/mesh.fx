//--------------------------------------------------------------------------------------
// File: Pick.fx
//
// The effect file for the Pick sample.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
float	Ka = .5;
float4	g_MaterialAmbientColor;		// Material's ambient color
float	Kd = .5;
float4	g_MaterialDiffuseColor;		// Material's diffuse color
float3	g_LightDir = float3(0, -0.70710678118654746, -0.70710678118654746);	// Light's direction in world space
										// (0, -1, -1): 앞 위
float4	g_LightDiffuse;			// Light's diffuse color
float3	g_LightDir2 = float3(+0.57735026918962584, -0.57735026918962584, +0.57735026918962584);	// (+1, -1, +1): 뒤 좌
float3	g_LightDir3 = float3(-0.57735026918962584, -0.57735026918962584, +0.57735026918962584);	// (-1, -1, +1): 뒤 우

// kEnum.h 에 있는 SHADING_TYPE 의 정의에 따라...
//	typedef enum {
//		SHADING_NONE	= 0,	// 단색으로 표시
//		SHADING_GREY	= 1,	// Grey 효과 (라이팅만 고려)
//		SHADING_TEXTURE	= 2,	// Texture 가 들어간 Color
//	} SHADING_TYPE;
#define SHADING_NONE	0
#define SHADING_GREY	1
#define SHADING_TEXTURE	2
DWORD	g__texture = SHADING_NONE;
DWORD	g__overlap = 0;
float	g__zDelta = 0;	// 원래 z 을 얼마나 변경할 지...: z -> z + g__zDelta

texture	g_MeshTexture;			// Color texture for mesh

float    g_fTime;			// App's time in seconds
float4x4 g_mWorld;			// World matrix for object
float4x4 g_mWorldViewProjection;	// World * View * Projection matrix

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler MeshTextureSampler =
sampler_state
{
	Texture = <g_MeshTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
//	AddressU = MIRROR;
};
// typedef enum _D3DFILLMODE {
//	D3DFILL_POINT		= 1,
//	D3DFILL_WIREFRAME	= 2,
//	D3DFILL_SOLID		= 3,
//	D3DFILL_FORCE_DWORD	= 0x7fffffff, /* force 32-bit size enum */
// } D3DFILLMODE;
#define D3DFILL_POINT		1
#define D3DFILL_WIREFRAME	2
DWORD	g__FillMode;
float	pointSize = 3.f;
float	fAlpha = .7f;
float4	pointColor = float4(1.f, 0.f, 0.f, 1.f);


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
	float4	Position : POSITION;	// vertex position
	float4	Diffuse : COLOR0;	// vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords
	float	Size : PSIZE;
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform and lighting
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderSceneVS(
	float3	vPos : POSITION,
	float3	vNormal : NORMAL,
	float2	vTexCoord0 : TEXCOORD0,
	float3	vColor : COLOR0
)
{
	VS_OUTPUT	Output;
	float3	vNormalWorldSpace;

	// Transform the position from object space to homogeneous projection space
	Output.Position = mul(float4(vPos, 1.f), g_mWorldViewProjection);

	// Transform the normal from object space to world space
	vNormalWorldSpace = normalize(mul(vNormal, (float3x3) g_mWorld));	// normal (world space)

	// Calc diffuse color
	if (g__FillMode == D3DFILL_POINT || g__FillMode == D3DFILL_WIREFRAME) {
		Output.Diffuse = pointColor;
	} else {
		if (g__texture != SHADING_NONE) {
			if (g__overlap) {	// g__showDefinedArea or g__showDefinedBodyElement
				Output.Diffuse.rgb = Kd * float4(vColor, 1.f) * g_LightDiffuse * (
					max(0, dot(vNormalWorldSpace, g_LightDir)) +
					max(0, dot(vNormalWorldSpace, g_LightDir2)) +
					max(0, dot(vNormalWorldSpace, g_LightDir3))
				) + Ka * g_MaterialAmbientColor;
			} else {
				if (0) {
					Output.Diffuse.rgb = Kd * g_MaterialDiffuseColor * g_LightDiffuse * max(0, dot(vNormalWorldSpace, g_LightDir)) + Ka * g_MaterialAmbientColor;
				} else {
					Output.Diffuse.rgb = Kd * g_MaterialDiffuseColor * g_LightDiffuse * (
						max(0, dot(vNormalWorldSpace, g_LightDir)) +
						max(0, dot(vNormalWorldSpace, g_LightDir2)) +
						max(0, dot(vNormalWorldSpace, g_LightDir3))
					) + Ka * g_MaterialAmbientColor;
				}
			}
		} else {
			Output.Diffuse = float4(vColor, .5f);
		}
	}
	Output.Diffuse.a = fAlpha;
	Output.Position.z += g__zDelta;

	// Just copy the texture coordinate through
	Output.TextureUV = vTexCoord0;
	Output.Size = pointSize;

	return Output;
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
	float4	RGBColor : COLOR0;	// Pixel color
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by modulating the texture's
// color with diffuse material color
//--------------------------------------------------------------------------------------
PS_OUTPUT
RenderScenePS(
	VS_OUTPUT	In
)
{
	PS_OUTPUT	Output;

	// Lookup mesh texture and modulate it with diffuse
	if (g__FillMode == D3DFILL_POINT || g__FillMode == D3DFILL_WIREFRAME) {
		Output.RGBColor = pointColor;
	} else {
		// Lookup mesh texture and modulate it with diffuse
		if (g__texture == SHADING_TEXTURE) {
In.Diffuse = float4(1, 1, 1, 1);	// 텍스쳐에 있는 값을 그대로 보여준다.
			Output.RGBColor = tex2D(MeshTextureSampler, In.TextureUV) * In.Diffuse;
//Output.RGBColor *= 2;	// 좀 더 밝게
		} else {
			Output.RGBColor = In.Diffuse;
		}
	}

	return Output;
}


//--------------------------------------------------------------------------------------
// Renders scene
//--------------------------------------------------------------------------------------
technique RenderScene
{
	pass P0
	{
		VertexShader = compile vs_2_0 RenderSceneVS();
		PixelShader  = compile ps_2_0 RenderScenePS();
	}
}

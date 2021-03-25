// FBX用ピクセルシェーダ (FbxModelPixel.hlsl)

// グローバル
cbuffer global : register(b0) {
	matrix	g_WorldViewProj;	// ワールド×ビュー×射影行列
	matrix	g_World;			// ワールド行列
	float4	g_cameraPos;		// 視点座標(ワールド空間)
	float4	g_lightDir;			// 光源方向(ワールド空間)
	float4	g_lightAmbient;		// 環境光
	float4	g_lightDiffuse;		// 拡散光
	float4	g_lightSpecular;	// 鏡面反射光
};

// マテリアル
cbuffer global2 : register(b1) {
	float4	g_Ambient;			// 環境色
	float4	g_Diffuse;			// 拡散色
	float4	g_Specular;			// 鏡面反射色
	float4	g_Emissive;			// 発光色
};

Texture2D    g_texture		: register(t0);	// テクスチャ
Texture2D    g_texEmissive	: register(t1);	// 発光テクスチャ
SamplerState g_sampler		: register(s0);	// サンプラ

// パラメータ
struct VS_OUTPUT {
	float4	Pos			: SV_Position;
	float2	Tex			: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float3	PosForPS	: TEXCOORD2;
};

//
// ピクセルシェーダ
//
float4 main(VS_OUTPUT input) : SV_Target0
{
	float3 Diff = g_Diffuse.rgb;
	float Alpha = g_Diffuse.a;
	if (g_Ambient.a != 0.0f) {								// テクスチャ有無
		float4 TexDiff = g_texture.Sample(g_sampler, input.Tex);
		Diff *= TexDiff.rgb;
		Alpha *= TexDiff.a;
	}
	if (Alpha <= 0.0f) discard;

	if (g_lightDir.x != 0.0f || g_lightDir.y != 0.0f || g_lightDir.z != 0.0f) {
		float3 L = normalize(-g_lightDir.xyz);					// 光源へのベクトル
		float3 N = normalize(input.Normal);						// 法線ベクトル
		float3 V = normalize(g_cameraPos.xyz - input.PosForPS);	// 視点へのベクトル
		float3 H = normalize(L + V);							// ハーフベクトル
		Diff = g_lightAmbient.rgb * g_Ambient.rgb +
			g_lightDiffuse.rgb * Diff * saturate(dot(L, N));	// 拡散色 + 環境色
		float3 Spec = g_Specular.rgb * g_lightSpecular.rgb *
			pow(saturate(dot(N, H)), g_Specular.a);				// 鏡面反射色
		Diff += Spec;
	}

	float3 Emis = g_Emissive.rgb;								// 発光色
	if (g_Emissive.a != 0.0f) {
		float4 Emis4 = g_texEmissive.Sample(g_sampler, input.Tex);
		Emis *= (Emis4.rgb * Emis4.a);
	}
	Diff += Emis;

	return float4(Diff, Alpha);
}

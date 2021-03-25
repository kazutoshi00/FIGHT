// FBX�p�s�N�Z���V�F�[�_ (FbxModelPixel.hlsl)

// �O���[�o��
cbuffer global : register(b0) {
	matrix	g_WorldViewProj;	// ���[���h�~�r���[�~�ˉe�s��
	matrix	g_World;			// ���[���h�s��
	float4	g_cameraPos;		// ���_���W(���[���h���)
	float4	g_lightDir;			// ��������(���[���h���)
	float4	g_lightAmbient;		// ����
	float4	g_lightDiffuse;		// �g�U��
	float4	g_lightSpecular;	// ���ʔ��ˌ�
};

// �}�e���A��
cbuffer global2 : register(b1) {
	float4	g_Ambient;			// ���F
	float4	g_Diffuse;			// �g�U�F
	float4	g_Specular;			// ���ʔ��ːF
	float4	g_Emissive;			// �����F
};

Texture2D    g_texture		: register(t0);	// �e�N�X�`��
Texture2D    g_texEmissive	: register(t1);	// �����e�N�X�`��
SamplerState g_sampler		: register(s0);	// �T���v��

// �p�����[�^
struct VS_OUTPUT {
	float4	Pos			: SV_Position;
	float2	Tex			: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float3	PosForPS	: TEXCOORD2;
};

//
// �s�N�Z���V�F�[�_
//
float4 main(VS_OUTPUT input) : SV_Target0
{
	float3 Diff = g_Diffuse.rgb;
	float Alpha = g_Diffuse.a;
	if (g_Ambient.a != 0.0f) {								// �e�N�X�`���L��
		float4 TexDiff = g_texture.Sample(g_sampler, input.Tex);
		Diff *= TexDiff.rgb;
		Alpha *= TexDiff.a;
	}
	if (Alpha <= 0.0f) discard;

	if (g_lightDir.x != 0.0f || g_lightDir.y != 0.0f || g_lightDir.z != 0.0f) {
		float3 L = normalize(-g_lightDir.xyz);					// �����ւ̃x�N�g��
		float3 N = normalize(input.Normal);						// �@���x�N�g��
		float3 V = normalize(g_cameraPos.xyz - input.PosForPS);	// ���_�ւ̃x�N�g��
		float3 H = normalize(L + V);							// �n�[�t�x�N�g��
		Diff = g_lightAmbient.rgb * g_Ambient.rgb +
			g_lightDiffuse.rgb * Diff * saturate(dot(L, N));	// �g�U�F + ���F
		float3 Spec = g_Specular.rgb * g_lightSpecular.rgb *
			pow(saturate(dot(N, H)), g_Specular.a);				// ���ʔ��ːF
		Diff += Spec;
	}

	float3 Emis = g_Emissive.rgb;								// �����F
	if (g_Emissive.a != 0.0f) {
		float4 Emis4 = g_texEmissive.Sample(g_sampler, input.Tex);
		Emis *= (Emis4.rgb * Emis4.a);
	}
	Diff += Emis;

	return float4(Diff, Alpha);
}

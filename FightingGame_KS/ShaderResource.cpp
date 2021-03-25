#include "ShaderResource.h"
#include <vector>
//#include "Defines.h"
#include "main.h"

// 実行エラー
#define FnAssert(fn, str) do { _ASSERT_EXPR(SUCCEEDED(fn), _CRT_WIDE(str)); } while(0)

const char* pVSPath[] =
{
	"data/shader/VertexShader.cso",
	"data/shader/BumpVS.cso",
	"data/shader/ProjShadowVS.cso",
	"data/shader/DepthWriteVS.cso",
	"data/shader/PMDVS.cso"
};
VertexShader::Layout Layouts[] =
{
	VertexShader::LAYOUT_PCUN,
	VertexShader::LAYOUT_PUNT,
	VertexShader::LAYOUT_PCUN,
	VertexShader::LAYOUT_PCUN,
	VertexShader::LAYOUT_PMD
};
static_assert(!(VS_MAX < _countof(pVSPath)), "VSKindへの定義追加忘れ");
static_assert(!(VS_MAX > _countof(pVSPath)), "VSPathへの読込ファイル追加忘れ");
static_assert(VS_MAX == _countof(Layouts), "頂点シェーダレイアウト指定忘れ");
const char* pPSPath[] =
{
	"data/shader/PixelShader.cso",
	"data/shader/PixelColorPS.cso",
	"data/shader/LambertPS.cso",
	"data/shader/PhongPS.cso",
	"data/shader/BumpPS.cso",
	"data/shader/ParallaxPS.cso",
	"data/shader/ParallaxOcclutionPS.cso",
	"data/shader/ProjShadowPS.cso",
	"data/shader/DepthWritePS.cso",
	"data/shader/DepthShadowPS.cso",
	"data/shader/PMDPS.cso",
	"data/shader/LightSourcePS.cso",
	"data/shader/DeferredWritePS.cso",
	"data/shader/DeferredLightPS.cso",
};
static_assert(!(PS_MAX < _countof(pPSPath)), "PSKindへの定義追加忘れ");
static_assert(!(PS_MAX > _countof(pPSPath)), L"PSPathへの読込ファイル追加忘れ");


std::vector<VertexShader*> g_vsList;
std::vector<PixelShader*> g_psList;

void InitShaderResource()
{
	g_vsList.resize(VS_MAX);
	for (int i = 0; i < VS_MAX; ++i)
	{
		g_vsList[i] = new VertexShader(Layouts[i]);
		FnAssert(g_vsList[i]->Create(pVSPath[i]), "頂点シェーダ読み込み失敗");
	}
	g_psList.resize(PS_MAX);
	for (int i = 0; i < PS_MAX; ++i)
	{
		g_psList[i] = new PixelShader();
		FnAssert(g_psList[i]->Create(pPSPath[i]), "ピクセルシェーダ読み込み失敗");
	}
}
void UninitShaderResource()
{
	for (int i = 0; i < VS_MAX; ++i) { delete g_vsList[i]; }
	for (int i = 0; i < PS_MAX; ++i) { delete g_psList[i]; }
}
void BindVS(VSKind vs)
{
	g_vsList[vs]->Bind();
}
void BindPS(PSKind ps)
{
	g_psList[ps]->Bind();
}

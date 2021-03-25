#include "SceneResult.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"
#include "SceneManager.h"

CSceneResult::CSceneResult()
{
}


CSceneResult::~CSceneResult()
{
}

HRESULT CSceneResult::Init()
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/resultLogo.png",		// ファイルの名前
		&m_pTexresultLogo);	// 読み込むメモリー
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/resultBG.jpg",		// ファイルの名前
		&m_pTexresultBG);	// 読み込むメモリー
	return hr;
}
void CSceneResult::Uninit()
{
	SAFE_RELEASE(m_pTexresultBG);
	SAFE_RELEASE(m_pTexresultLogo);
}
void CSceneResult::Update()
{
	if (GetKeyTrigger(VK_SPACE))
	{
		ChangeScene(TITLE);
	}
}
void CSceneResult::Draw()
{
	SetZBuffer(false);
	SetCullMode(CULLMODE_CCW);	// 背面カリング(裏を描かない)
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexresultBG);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexresultLogo);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
	SetCullMode(CULLMODE_CW);	// 前面カリング(表を描かない)
	SetZWrite(true);
}

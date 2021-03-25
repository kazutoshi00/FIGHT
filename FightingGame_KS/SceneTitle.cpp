#include "SceneTitle.h"
#include "SceneManager.h"
#include "Texture.h"
#include "polygon.h"
#include "input.h"

CSceneTitle::CSceneTitle()
{
}


CSceneTitle::~CSceneTitle()
{
}

HRESULT CSceneTitle::Init() 
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/TitleLogo.png",		// ファイルの名前
		&m_pTexTitleLogo);	// 読み込むメモリー
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/TitleGuide.png",		// ファイルの名前
		&m_pTexTitleGuide);	// 読み込むメモリー
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/TitleBG.jpg",		// ファイルの名前
		&m_pTexTitleBG);	// 読み込むメモリー

	m_fTitleGuideAlpha = 0.0f;
	m_bTitleGuideAlphaFlg = true;

	return hr;
}
void CSceneTitle::Uninit()
{
	SAFE_RELEASE(m_pTexTitleLogo);
	SAFE_RELEASE(m_pTexTitleGuide);
	SAFE_RELEASE(m_pTexTitleBG);
}
void CSceneTitle::Update()
{
	if (m_bTitleGuideAlphaFlg)
	{
		m_fTitleGuideAlpha += 0.01f;
		if (m_fTitleGuideAlpha >= 1.0f)
			m_bTitleGuideAlphaFlg = false;
	}
	else
	{
		m_fTitleGuideAlpha -= 0.01f;
		if (m_fTitleGuideAlpha <= 0.0f)
			m_bTitleGuideAlphaFlg = true;
	}
	if (GetKeyTrigger(VK_SPACE))
	{
		ChangeScene(GAME);
	}
}

void CSceneTitle::Draw()
{
	// Zバッファ無効
	SetZBuffer(false);
	SetCullMode(CULLMODE_CCW);	// 背面カリング(裏を描かない)
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonColor(1.0f,1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexTitleBG);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 200.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexTitleLogo);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(500.0f, 100.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, -100.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(m_fTitleGuideAlpha);
	SetPolygonTexture(m_pTexTitleGuide);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
	SetCullMode(CULLMODE_CW);	// 前面カリング(表を描かない)
	SetZWrite(true);
}
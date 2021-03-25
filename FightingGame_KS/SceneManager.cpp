#include "SceneManager.h"
#include "Texture.h"
#include "polygon.h"
#include "SceneBase.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneResult.h"

CSceneManager* g_pSceneManager;
CSceneManager* GetSceneManager() { return g_pSceneManager; }
void CreateSceneManager() { if (!g_pSceneManager)g_pSceneManager = new CSceneManager; }
void DeleteSceneManager() { SAFE_DELETE(g_pSceneManager); }


void ChangeScene(CSceneManager* pSM, enum SCENE_KIND scene)
{
	pSM->m_bChangeSceneflg = false;
	pSM->Uninit();
	delete pSM->m_pNowScene;
	pSM->m_nowscene = pSM->m_storescene;
	switch (scene)
	{
	case TITLE:
		pSM->m_pNowScene = new CSceneTitle;
		break;
	case GAME:
		pSM->m_pNowScene = new CSceneGame;
		break;
	case RESULT:
		pSM->m_pNowScene = new CSceneResult;
		break;
	default:break;
	}
	pSM->Init();

}

CSceneManager::CSceneManager()
{
	m_bChangeSceneflg = false;
	m_pNowScene = new CSceneTitle;

	m_nowscene = TITLE;
	m_storescene = TITLE;
	m_pfade = new CFade;
}


CSceneManager::~CSceneManager()
{
	SAFE_DELETE(m_pfade);
	SAFE_DELETE(m_pNowScene);
}


HRESULT CSceneManager::Init()
{
	return m_pNowScene->Init();
}
void CSceneManager::Uninit()
{
	m_pNowScene->Uninit();
}
void CSceneManager::Update()
{
	if (!m_pfade->Isfade())
	{
		m_pNowScene->Update();
	}
	else if (m_pfade->Isfadein())
	{
		if (m_bChangeSceneflg)
		{
			ChangeScene(this, m_storescene);
		}
	}
	m_pfade->Update();
}
void CSceneManager::Draw()
{
	m_pNowScene->Draw();
	m_pfade->Draw();
}

void CSceneManager::SetNextScene(SCENE_KIND scene)
{
	m_bChangeSceneflg = true;
	m_storescene = scene;
	m_pfade->StartFade(FADE_NORMAL);
}

void ChangeScene(enum SCENE_KIND scene)
{
	g_pSceneManager->SetNextScene(scene);
}

CFade::CFade()
{
	Init();
}

CFade::~CFade()
{
	Uninit();
}

void CFade::Init()
{

	char fadefilename[FADE_KIND_MAX][256]= {
		{"data/texture/fade.png"},
	};

	ID3D11Device* pDevice = GetDevice();
	for (int i = 0; i < FADE_KIND_MAX; i++)
	{
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		fadefilename[i],		// ファイルの名前
		&m_pTexFade[i]);	// 読み込むメモリー

	}

	m_bIsfade = false;
	m_bIsfadeout = false;
	m_bIsfadein = false;
	m_nowfade = FADE_NORMAL;
	
	m_fadeAlpha = 0.0f;
}

void CFade::Uninit()
{
	for (int i = 0; i < FADE_KIND_MAX; i++)
	{
		SAFE_RELEASE(m_pTexFade[i]);
	}
}

void CFade::Update()
{
	if (!m_bIsfade)return;
	if (m_bIsfadeout)
	{
		m_fadeAlpha += 0.02f;
		if (m_fadeAlpha >= 1.0f)
		{
			m_fadeAlpha = 1.0f;
			m_bIsfadeout = false;
			m_bIsfadein = true;
		}
	}
	else if (m_bIsfadein)
	{
		m_fadeAlpha -= 0.02f;
		if (m_fadeAlpha <= 0.0f)
		{
			m_fadeAlpha = 0.0f;
			m_bIsfadein = false;
			m_bIsfade = false;
		}
	}
}

void CFade::Draw()
{
	SetZBuffer(false);
	SetCullMode(CULLMODE_CCW);	// 背面カリング(裏を描かない)
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(m_fadeAlpha);
	SetPolygonTexture(m_pTexFade[m_nowfade]);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
	SetCullMode(CULLMODE_CW);	// 前面カリング(表を描かない)
	SetZWrite(true);
}

void CFade::StartFade(enum FADE_KIND fade)
{
	if (m_bIsfade)return;
	m_bIsfade = true;
	m_bIsfadeout = true;
	m_nowfade = fade;
}
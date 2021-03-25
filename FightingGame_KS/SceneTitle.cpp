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
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/TitleLogo.png",		// �t�@�C���̖��O
		&m_pTexTitleLogo);	// �ǂݍ��ރ������[
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/TitleGuide.png",		// �t�@�C���̖��O
		&m_pTexTitleGuide);	// �ǂݍ��ރ������[
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/TitleBG.jpg",		// �t�@�C���̖��O
		&m_pTexTitleBG);	// �ǂݍ��ރ������[

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
	// Z�o�b�t�@����
	SetZBuffer(false);
	SetCullMode(CULLMODE_CCW);	// �w�ʃJ�����O(����`���Ȃ�)
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonColor(1.0f,1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexTitleBG);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 200.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexTitleLogo);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(500.0f, 100.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, -100.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(m_fTitleGuideAlpha);
	SetPolygonTexture(m_pTexTitleGuide);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
	SetCullMode(CULLMODE_CW);	// �O�ʃJ�����O(�\��`���Ȃ�)
	SetZWrite(true);
}
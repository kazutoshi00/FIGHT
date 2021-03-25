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
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/resultLogo.png",		// �t�@�C���̖��O
		&m_pTexresultLogo);	// �ǂݍ��ރ������[
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/resultBG.jpg",		// �t�@�C���̖��O
		&m_pTexresultBG);	// �ǂݍ��ރ������[
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
	SetCullMode(CULLMODE_CCW);	// �w�ʃJ�����O(����`���Ȃ�)
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexresultBG);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonColor(1.0f, 1.0f, 1.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(m_pTexresultLogo);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
	SetCullMode(CULLMODE_CW);	// �O�ʃJ�����O(�\��`���Ȃ�)
	SetZWrite(true);
}

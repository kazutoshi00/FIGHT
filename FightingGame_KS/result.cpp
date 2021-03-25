#include "result.h"
#include "Texture.h"
#include "polygon.h"


ID3D11ShaderResourceView* g_pTexresultLogo;
ID3D11ShaderResourceView* g_pTexresultBG;

void InitResult()
{
	ID3D11Device* pDevice = GetDevice();
	CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/resultLogo.png",		// �t�@�C���̖��O
		&g_pTexresultLogo);	// �ǂݍ��ރ������[
	CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		"data/texture/resultBG.jpg",		// �t�@�C���̖��O
		&g_pTexresultBG);	// �ǂݍ��ރ������[
}
void UninitResult()
{
	SAFE_RELEASE(g_pTexresultBG);
	SAFE_RELEASE(g_pTexresultLogo);
}
void UpdateResult()
{

}
void DrawResult()
{
	SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexresultBG);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexresultLogo);
	SetPolygonFrameSize(1, 1);	// �ǂݍ��񂾉摜�̕�����
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
}

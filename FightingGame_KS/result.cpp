#include "result.h"
#include "Texture.h"
#include "polygon.h"


ID3D11ShaderResourceView* g_pTexresultLogo;
ID3D11ShaderResourceView* g_pTexresultBG;

void InitResult()
{
	ID3D11Device* pDevice = GetDevice();
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/resultLogo.png",		// ファイルの名前
		&g_pTexresultLogo);	// 読み込むメモリー
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/resultBG.jpg",		// ファイルの名前
		&g_pTexresultBG);	// 読み込むメモリー
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
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(600.0f, 200.0f);
	SetPolygonPos(0.0f, 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexresultLogo);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
}

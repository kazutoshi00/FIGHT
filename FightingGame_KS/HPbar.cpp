#include "HPbar.h"
#include "Texture.h"
#include "polygon.h"

#define HPBAR_SIZE (XMFLOAT2(500.0f,50.0f))
#define HPBAR_POS_PLAYER (XMFLOAT2(-350.0f,300.0f))
#define HPBAR_POS_ENEMY (XMFLOAT2(350.0f,300.0f))

ID3D11ShaderResourceView* g_pTexPlayerBase;
ID3D11ShaderResourceView* g_pTexPlayerBar;
ID3D11ShaderResourceView* g_pTexEnemyBase;
ID3D11ShaderResourceView* g_pTexEnemyBar;


float g_PlyerHP;
float g_PlyerHPMAX;
float g_EnemyHP;
float g_EnemyHPMAX;


void InitHP()
{
	ID3D11Device* pDevice = GetDevice();
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/HPbase.png",		// ファイルの名前
		&g_pTexPlayerBase);	// 読み込むメモリー
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/HPbase.png",		// ファイルの名前
		&g_pTexEnemyBase);	// 読み込むメモリー
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/HPbar.png",		// ファイルの名前
		&g_pTexPlayerBar);	// 読み込むメモリー
	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		"data/texture/HPbar.png",		// ファイルの名前
		&g_pTexEnemyBar);	// 読み込むメモリー
}
void UninitHP()
{
	SAFE_RELEASE(g_pTexPlayerBase)
	SAFE_RELEASE(g_pTexPlayerBar)
	SAFE_RELEASE(g_pTexEnemyBase)
	SAFE_RELEASE(g_pTexEnemyBar)
}
void UpdateHP()
{

}
void DrawHP()
{

	SetPolygonSize(HPBAR_SIZE.x, HPBAR_SIZE.y);
	SetPolygonPos(HPBAR_POS_PLAYER.x, HPBAR_POS_PLAYER.y);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexPlayerBase);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(HPBAR_SIZE.x, HPBAR_SIZE.y);
	SetPolygonPos(HPBAR_POS_ENEMY.x, HPBAR_POS_ENEMY.y);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexEnemyBase);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());


	SetPolygonSize(HPBAR_SIZE.x * (g_PlyerHP / g_PlyerHPMAX), HPBAR_SIZE.y);
	SetPolygonPos(HPBAR_POS_PLAYER.x - (HPBAR_SIZE.x * (1.0f - (g_PlyerHP / g_PlyerHPMAX))) / 2, HPBAR_POS_PLAYER.y);
	SetPolygonColor(1.0f - (g_PlyerHP / g_PlyerHPMAX), (g_PlyerHP / g_PlyerHPMAX), 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexPlayerBar);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());

	SetPolygonSize(HPBAR_SIZE.x * (g_EnemyHP/ g_EnemyHPMAX), HPBAR_SIZE.y);
	SetPolygonPos(HPBAR_POS_ENEMY.x - (HPBAR_SIZE.x * (1.0f - (g_EnemyHP / g_EnemyHPMAX))) / 2, HPBAR_POS_ENEMY.y);
	SetPolygonColor(1.0f - (g_EnemyHP / g_EnemyHPMAX), (g_EnemyHP / g_EnemyHPMAX), 0.0f);
	SetPolygonAngle(0.0f);
	SetPolygonAlpha(1.0f);
	SetPolygonTexture(g_pTexEnemyBar);
	SetPolygonFrameSize(1, 1);	// 読み込んだ画像の分割数
	SetPolygonUV(1.0f, 1.0f);
	UpdatePolygon();
	DrawPolygon(GetDeviceContext());
}

void SetPlayerHP(int hp) { g_PlyerHP = hp; }
void SetPlayerHPMAX(int hp) { g_PlyerHPMAX = hp; }
void SetEnemyHP(int hp) { g_EnemyHP = hp; }
void SetEnemyHPMAX(int hp) { g_EnemyHPMAX = hp; }
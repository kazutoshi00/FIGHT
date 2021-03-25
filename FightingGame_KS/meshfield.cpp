#include "meshfield.h"
#include "mesh.h"
#include "Texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_FILENAME	"data/texture/field003.jpg"	// 読み込むテクスチャファイル名

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MESH		g_meshfield;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// テクスチャ マトリックス初期化
	XMStoreFloat4x4(&g_meshfield.mtxTexture, XMMatrixIdentity());

	// 位置、向きの初期設定
	g_meshfield.pos = pos;
	g_meshfield.rot = rot;
	g_meshfield.size = XMFLOAT3(1,1,1);

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
							   TEXTURE_FILENAME,		// ファイルの名前
							   &g_meshfield.pTexture);	// 読み込むメモリー
	if (FAILED(hr))
		return hr;

	// 頂点数の設定
	g_meshfield.nNumVertex = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// インデックス数の設定
	g_meshfield.nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// ポリゴン数の設定
	int nNumPolygon = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// オブジェクトの頂点バッファを生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_meshfield.nNumVertex];

	// オブジェクトのインデックスバッファを生成
	int* pIndexWk = new int[g_meshfield.nNumIndex];

	// 頂点バッファの中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
#if 0
	const float texSizeX = 1.0f / nNumBlockX;
	const float texSizeZ = 1.0f / nNumBlockZ;
#else
	const float texSizeX = 1.0f;
	const float texSizeZ = 1.0f;
#endif

	for (int nCntVtxZ = 0; nCntVtxZ < nNumBlockZ + 1; ++nCntVtxZ) {
		const float ANGLE = XM_2PI / nNumBlockX;

		for (int nCntVtxX = 0; nCntVtxX < nNumBlockX + 1; ++nCntVtxX) {
			// 頂点座標の設定
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.x = -(nNumBlockX / 2.0f) * fSizeBlockX + nCntVtxX * fSizeBlockX;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.y = 0.0f;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.z = (nNumBlockZ / 2.0f) * fSizeBlockZ - nCntVtxZ * fSizeBlockZ;

			// 法線の設定
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].nrm = XMFLOAT3(0.0f, 1.0, 0.0f);
			
			// 反射光の設定
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxZ;
		}
	}

	// インデックスバッファの中身を埋める
	int* pIdx = pIndexWk;

	int nCntIdx = 0;
	for (int nCntVtxZ = 0; nCntVtxZ < nNumBlockZ; ++nCntVtxZ) {
		if (nCntVtxZ > 0) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockX + 1);
			++nCntIdx;
		}

		for (int nCntVtxX = 0; nCntVtxX < nNumBlockX + 1; ++nCntVtxX) {
			pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockX + 1) + nCntVtxX;
			++nCntIdx;
			pIdx[nCntIdx] = nCntVtxZ * (nNumBlockX + 1) + nCntVtxX;
			++nCntIdx;
		}

		if (nCntVtxZ < nNumBlockZ - 1) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = nCntVtxZ * (nNumBlockX + 1) + nNumBlockX;
			++nCntIdx;
		}
	}

	// 頂点バッファ/インデックス バッファ生成
	hr = MakeMeshVertex(pDevice, &g_meshfield, pVertexWk, pIndexWk);

	// 一時配列解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMeshField(void)
{
	// メッシュ解放
	ReleaseMesh(&g_meshfield);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMeshField(void)
{
	// メッシュ更新
	UpdateMesh(&g_meshfield);
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMeshField(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// メッシュ描画
	DrawMesh(pDeviceContext, &g_meshfield,true);
}

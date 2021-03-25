#include "OBB.h"
#include "Light.h"
#include "Texture.h"

// インデックス バッファ生成
static int g_index[36] = { 0, 1, 2, 2, 1, 3,
	4, 5, 6, 6, 5, 7,
	8, 9, 10, 10, 9, 11,
	12, 13, 14, 14, 13, 15,
	16, 17, 18, 18, 17, 19,
	20, 21, 22, 22, 21, 23,
};

#define TEXTURE_FILENAME L"data/TEXTURE/yellow.png"
static ID3D11ShaderResourceView*	g_pTextureBox;	// テクスチャ読み込み場所


//static int g_index[34] = { 0,2,1,3,3,4,4,6,5,7,7,8,8,10,9,11,11,12,12,14,13,15,15,16,16,18,17,19,19,20,20,22,21,23};
// 頂点バッファ生成
static XMFLOAT3 g_pos[24] = {
	{-1,  1, -1}, { 1,  1, -1}, {-1, -1, -1}, { 1, -1, -1},
	{ 1,  1, -1}, { 1,  1,  1}, { 1, -1, -1}, { 1, -1,  1},
	{ 1,  1,  1}, {-1,  1,  1}, { 1, -1,  1}, {-1, -1,  1},
	{-1,  1,  1}, {-1,  1, -1}, {-1, -1,  1}, {-1, -1, -1},
	{-1,  1,  1}, { 1,  1,  1}, {-1,  1, -1}, { 1,  1, -1},
	{-1, -1, -1}, { 1, -1, -1}, {-1, -1,  1}, { 1, -1,  1},
};
static XMFLOAT3 g_dir[6] = {
	{ 0,  0, -1},
	{ 1,  0,  0},
	{ 0,  0,  1},
	{-1,  0,  0},
	{ 0,  1,  0},
	{ 0, -1,  0},
};

FLOAT LenSegOnSeparateAxis(XMVECTOR *Sep, XMVECTOR *e1, XMVECTOR *e2, XMVECTOR *e3 = 0)
{
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	FLOAT r1 = fabs(XMVectorGetX(XMVector3Dot(*Sep, *e1)));
	FLOAT r2 = fabs(XMVectorGetX(XMVector3Dot(*Sep, *e2)));
	FLOAT r3 = e3 ? (fabs(XMVectorGetX(XMVector3Dot(*Sep, *e3)))) : 0;
	return r1 + r2 + r3;
}

bool ColObbs(COBB &obb1, COBB &obb2)
{
	if (!obb1.IsUse() || !obb2.IsUse())
		return false;
	// 各方向ベクトルの確保
  // （N***:標準化方向ベクトル）
	XMVECTOR NAe1 = obb1.GetDirect(0), Ae1 = NAe1 * obb1.GetLen_W(0);
	XMVECTOR NAe2 = obb1.GetDirect(1), Ae2 = NAe2 * obb1.GetLen_W(1);
	XMVECTOR NAe3 = obb1.GetDirect(2), Ae3 = NAe3 * obb1.GetLen_W(2);
	XMVECTOR NBe1 = obb2.GetDirect(0), Be1 = NBe1 * obb2.GetLen_W(0);
	XMVECTOR NBe2 = obb2.GetDirect(1), Be2 = NBe2 * obb2.GetLen_W(1);
	XMVECTOR NBe3 = obb2.GetDirect(2), Be3 = NBe3 * obb2.GetLen_W(2);
	XMFLOAT3 Interval = XMFLOAT3(obb1.GetPos_W().x - obb2.GetPos_W().x, obb1.GetPos_W().y - obb2.GetPos_W().y, obb1.GetPos_W().z - obb2.GetPos_W().z);

	// 分離軸 : Ae1
	FLOAT rA = XMVectorGetX(XMVector3Length(Ae1));
	FLOAT rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	FLOAT L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe1)));
	if (L > rA + rB)
		return false; // 衝突していない

	 // 分離軸 : Ae2
	rA = XMVectorGetX(XMVector3Length(Ae2));
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe2)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Ae3
	rA = XMVectorGetX(XMVector3Length(Ae3));
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe3)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be1));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe1)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be2));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe2)));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be3));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe3)));
	if (L > rA + rB)
		return false;


	// 分離軸 : C11
	XMVECTOR Cross;
	Cross = XMVector3Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C12
	Cross = XMVector3Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C13
	Cross = XMVector3Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C21
	Cross = XMVector3Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C22
	Cross = XMVector3Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C23
	Cross = XMVector3Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C31
	Cross = XMVector3Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C32
	Cross = XMVector3Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離軸 : C33
	Cross = XMVector3Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// 分離平面が存在しないので「衝突している」
	return true;
}

XMVECTOR COBB::GetDirect(int elem)  // 指定軸番号の方向ベクトルを取得
{
	return m_NormaDirect[elem];
}
float COBB::GetLen_W(int elem)          // 指定軸方向の長さを取得
{
	return m_fLength[elem];
}
XMFLOAT3 COBB::GetPos_W()             // 位置を取得
{
	return GetWorldPosition();
}

void COBB::SetSize(XMFLOAT3 size)
{
	if (size.x < 0.0f)size.x = 0.0f;
	if (size.y < 0.0f)size.y = 0.0f; 
	if (size.z < 0.0f)size.z = 0.0f;
	m_localscale = size;
	//SetMesh();
}
void COBB::SetPos(XMFLOAT3 pos)
{
	m_localposition = pos;
}
void COBB::SetRot(XMFLOAT3 rot)
{
	while (rot.x > 180.0f) { rot.x -= 360.0f; }
	while (rot.x < -180.0f) { rot.x += 360.0f; }
	while (rot.y > 180.0f) { rot.y -= 360.0f; }
	while (rot.y < -180.0f) { rot.y += 360.0f; }
	while (rot.z > 180.0f) { rot.z -= 360.0f; }
	while (rot.z < -180.0f) { rot.z += 360.0f; }
	m_localrotation = rot;
}
XMFLOAT3 COBB::GetSize()
{
	return m_localscale;
}
XMFLOAT3 COBB::GetPos()
{
	return m_localposition;
}
XMFLOAT3 COBB::GetRot()
{
	return m_localrotation;
}

void COBB::SetMesh()
{
	ID3D11Device* pDevice = GetDevice();
	MESH* pMesh = m_pMesh;
	// オブジェクトの頂点バッファを生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// オブジェクトのインデックスバッファを生成
	int* pIndexWk = new int[pMesh->nNumIndex];

	for (int i = 0; i < 36; i++)
	{
		pIndexWk[i] = g_index[i];
	}
	for (int i = 0; i < 24; ++i) {
		pVertexWk[i].vtx.x = GetWorldScale().x * g_pos[i].x;
		pVertexWk[i].vtx.y = GetWorldScale().y * g_pos[i].y;
		pVertexWk[i].vtx.z = GetWorldScale().z * g_pos[i].z;
		pVertexWk[i].nrm = g_dir[i / 4];
	}

	MakeMeshVertex(pDevice, pMesh, pVertexWk, pIndexWk);
	// 一時配列解放
	delete[] pIndexWk;
	delete[] pVertexWk;

}

COBB::COBB()
{
	m_pMesh = new MESH();
	
}


COBB::~COBB()
{
	delete m_pMesh;
}

HRESULT COBB::Init()
{
	ID3D11Device* pDevice = GetDevice();
	MESH* pMesh = m_pMesh;

	CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		TEXTURE_FILENAME,		// ファイルの名前
	&g_pTextureBox);	// 読み込むメモリー

	pMesh->pTexture = g_pTextureBox;
	pMesh->pos = GetWorldPosition();
	pMesh->rot = GetWorldRotation();
	pMesh->nNumVertex = 24;
	pMesh->nNumIndex = 36;

	pMesh->primitiveType = PT_TRIANGLE;
	MATERIAL* pMaterial = new MATERIAL;
	pMaterial->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	pMaterial->Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	pMaterial->Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	pMaterial->Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	pMaterial->Power = 0.0f;

	pMesh->pMaterial = pMaterial;
	

	// オブジェクトの頂点バッファを生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// オブジェクトのインデックスバッファを生成
	int* pIndexWk = new int[pMesh->nNumIndex];


	for (int i = 0; i < 36; i++)
	{
		pIndexWk[i] = g_index[i];
	}
	for (int i = 0; i < 24; ++i) {
		pVertexWk[i].vtx.x = GetWorldScale().x * g_pos[i].x;
		pVertexWk[i].vtx.y = GetWorldScale().y * g_pos[i].y;
		pVertexWk[i].vtx.z = GetWorldScale().z * g_pos[i].z;
		pVertexWk[i].nrm = g_dir[i / 4];
		pVertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		pVertexWk[i].tex.x = ((i % 4) / 2) == 0 ? 0.0f : 1.0f;
		pVertexWk[i].tex.y = ((i % 4) % 2) == 0 ? 0.0f : 1.0f;
	}


	
	MakeMeshVertex(pDevice, pMesh, pVertexWk, pIndexWk);
	// 一時配列解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return S_OK;
}
void COBB::Uninit()
{
	MESH* pMesh;

	pMesh = m_pMesh;

	SAFE_DELETE(pMesh->pMaterial);
	pMesh->pTexture = nullptr;
	// メッシュ開放
	ReleaseMesh(pMesh);

	// テクスチャの開放
	SAFE_RELEASE(g_pTextureBox);
}
void COBB::Update()
{	
	if (IsUpdated()) { return; }
	// 先に行列計算
	CObject::Update();

	// ---オブジェクトの姿勢から各項目のセット--------------------------------------------------------------------
	XMFLOAT3 edgeposX, edgeposY, edgeposZ;
	XMFLOAT3 wepX, wepY, wepZ;
	float radX = XMConvertToRadians(GetWorldRotation().x), radY = XMConvertToRadians(GetWorldRotation().y), radZ = XMConvertToRadians(GetWorldRotation().z);

	// ---X軸の端頂点座標計算---
	edgeposX.x = GetWorldScale().x;
	edgeposX.y = 0.0f;
	edgeposX.z = 0.0f;
	// Z軸回転
	wepX.x = edgeposX.x * cosf(radZ) + edgeposX.y * sinf(radZ);
	wepX.y = edgeposX.x * -sinf(radZ) + edgeposX.y * cosf(radZ);
	wepX.z = edgeposX.z;
	edgeposX = wepX;

	// X軸回転
	wepX.x = edgeposX.x;
	wepX.y = edgeposX.y *cosf(radX) + edgeposX.z * sinf(radX);
	wepX.z = edgeposX.y * -sinf(radX) + edgeposX.z * cosf(radX);
	edgeposX = wepX;

	// Y軸回転
	wepX.x = edgeposX.x * cosf(radY) + edgeposX.z * -sinf(radY);
	wepX.y = edgeposX.y;
	wepX.z = edgeposX.x * sinf(radY) + edgeposX.z * cosf(radY);
	edgeposX = wepX;

	// ---Y軸の端頂点座標計算---
	edgeposY.x = 0.0f;
	edgeposY.y = GetWorldScale().y;
	edgeposY.z = 0.0f;
	// Z軸回転
	wepY.x = edgeposY.x * cosf(radZ) + edgeposY.y * sinf(radZ);
	wepY.y = edgeposY.x * -sinf(radZ) + edgeposY.y * cosf(radZ);
	wepY.z = edgeposY.z;
	edgeposY = wepY;

	// X軸回転
	wepY.x = edgeposY.x;
	wepY.y = edgeposY.y *cosf(radX) + edgeposY.z * sinf(radX);
	wepY.z = edgeposY.y * -sinf(radX) + edgeposY.z * cosf(radX);
	edgeposY = wepY;

	// Y軸回転
	wepY.x = edgeposY.x * cosf(radY) + edgeposY.z * -sinf(radY);
	wepY.y = edgeposY.y;
	wepY.z = edgeposY.x * sinf(radY) + edgeposY.z * cosf(radY);
	edgeposY = wepY;

	// ---Z軸の端頂点座標計算---
	edgeposZ.x = 0.0f;
	edgeposZ.y = 0.0f;
	edgeposZ.z = GetWorldScale().z;
	// Z軸回転
	wepZ.x = edgeposZ.x * cosf(radZ) + edgeposZ.y * sinf(radZ);
	wepZ.y = edgeposZ.x * -sinf(radZ) + edgeposZ.y * cosf(radZ);
	wepZ.z = edgeposZ.z;
	edgeposZ = wepZ;

	// X軸回転
	wepZ.x = edgeposZ.x;
	wepZ.y = edgeposZ.y *cosf(radX) + edgeposZ.z * sinf(radX);
	wepZ.z = edgeposZ.y * -sinf(radX) + edgeposZ.z * cosf(radX);
	edgeposZ = wepZ;

	// Y軸回転
	wepZ.x = edgeposZ.x * cosf(radY) + edgeposZ.z * -sinf(radY);
	wepZ.y = edgeposZ.y;
	wepZ.z = edgeposZ.x * sinf(radY) + edgeposZ.z * cosf(radY);
	edgeposZ = wepZ;


	// ベクトルを計算
	XMFLOAT3 vecX, vecY, vecZ;
	vecX = XMFLOAT3(edgeposX.x, edgeposX.y, edgeposX.z);
	vecY = XMFLOAT3(edgeposY.x, edgeposY.y, edgeposY.z);
	vecZ = XMFLOAT3(edgeposZ.x, edgeposZ.y, edgeposZ.z);
	// 単位ベクトルを格納
	m_NormaDirect[0] = XMVector3Normalize(XMLoadFloat3(&vecX));
	m_NormaDirect[1] = XMVector3Normalize(XMLoadFloat3(&vecY));
	m_NormaDirect[2] = XMVector3Normalize(XMLoadFloat3(&vecZ));

	// 各ベクトルの長さを格納
	m_fLength[0] = sqrtf(vecX.x * vecX.x + vecX.y * vecX.y + vecX.z * vecX.z);
	m_fLength[1] = sqrtf(vecY.x * vecY.x + vecY.y * vecY.y + vecY.z * vecY.z);
	m_fLength[2] = sqrtf(vecZ.x * vecZ.x + vecZ.y * vecZ.y + vecZ.z * vecZ.z);

	// ------------------------------------------------------------------------------------------------------------


	
	
	
}
void COBB::Draw()
{
	CObject::Draw();	// アップデート完了フラグをおろす
	if (!m_bDispHitbox)return;
	if (!m_bUse)return;
	// 現在の姿勢をメッシュにセット
	m_pMesh->pos = GetWorldPosition();
	m_pMesh->rot = XMFLOAT3(XMConvertToRadians(GetWorldRotation().x), XMConvertToRadians(GetWorldRotation().y), XMConvertToRadians(GetWorldRotation().z));
	m_pMesh->size = GetWorldScale();
	UpdateMesh(m_pMesh);
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	GetCLight()->SetLightFalse();
	// メッシュ描画
	DrawMesh(pDeviceContext, m_pMesh);
	GetCLight()->SetLightTrue();
}
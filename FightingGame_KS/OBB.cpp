#include "OBB.h"
#include "Light.h"
#include "Texture.h"

// �C���f�b�N�X �o�b�t�@����
static int g_index[36] = { 0, 1, 2, 2, 1, 3,
	4, 5, 6, 6, 5, 7,
	8, 9, 10, 10, 9, 11,
	12, 13, 14, 14, 13, 15,
	16, 17, 18, 18, 17, 19,
	20, 21, 22, 22, 21, 23,
};

#define TEXTURE_FILENAME L"data/TEXTURE/yellow.png"
static ID3D11ShaderResourceView*	g_pTextureBox;	// �e�N�X�`���ǂݍ��ݏꏊ


//static int g_index[34] = { 0,2,1,3,3,4,4,6,5,7,7,8,8,10,9,11,11,12,12,14,13,15,15,16,16,18,17,19,19,20,20,22,21,23};
// ���_�o�b�t�@����
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
	// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
	// ������Sep�͕W��������Ă��邱��
	FLOAT r1 = fabs(XMVectorGetX(XMVector3Dot(*Sep, *e1)));
	FLOAT r2 = fabs(XMVectorGetX(XMVector3Dot(*Sep, *e2)));
	FLOAT r3 = e3 ? (fabs(XMVectorGetX(XMVector3Dot(*Sep, *e3)))) : 0;
	return r1 + r2 + r3;
}

bool ColObbs(COBB &obb1, COBB &obb2)
{
	if (!obb1.IsUse() || !obb2.IsUse())
		return false;
	// �e�����x�N�g���̊m��
  // �iN***:�W���������x�N�g���j
	XMVECTOR NAe1 = obb1.GetDirect(0), Ae1 = NAe1 * obb1.GetLen_W(0);
	XMVECTOR NAe2 = obb1.GetDirect(1), Ae2 = NAe2 * obb1.GetLen_W(1);
	XMVECTOR NAe3 = obb1.GetDirect(2), Ae3 = NAe3 * obb1.GetLen_W(2);
	XMVECTOR NBe1 = obb2.GetDirect(0), Be1 = NBe1 * obb2.GetLen_W(0);
	XMVECTOR NBe2 = obb2.GetDirect(1), Be2 = NBe2 * obb2.GetLen_W(1);
	XMVECTOR NBe3 = obb2.GetDirect(2), Be3 = NBe3 * obb2.GetLen_W(2);
	XMFLOAT3 Interval = XMFLOAT3(obb1.GetPos_W().x - obb2.GetPos_W().x, obb1.GetPos_W().y - obb2.GetPos_W().y, obb1.GetPos_W().z - obb2.GetPos_W().z);

	// ������ : Ae1
	FLOAT rA = XMVectorGetX(XMVector3Length(Ae1));
	FLOAT rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	FLOAT L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe1)));
	if (L > rA + rB)
		return false; // �Փ˂��Ă��Ȃ�

	 // ������ : Ae2
	rA = XMVectorGetX(XMVector3Length(Ae2));
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe2)));
	if (L > rA + rB)
		return false;

	// ������ : Ae3
	rA = XMVectorGetX(XMVector3Length(Ae3));
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NAe3)));
	if (L > rA + rB)
		return false;

	// ������ : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be1));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe1)));
	if (L > rA + rB)
		return false;

	// ������ : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be2));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe2)));
	if (L > rA + rB)
		return false;

	// ������ : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = XMVectorGetX(XMVector3Length(Be3));
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), NBe3)));
	if (L > rA + rB)
		return false;


	// ������ : C11
	XMVECTOR Cross;
	Cross = XMVector3Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C12
	Cross = XMVector3Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C13
	Cross = XMVector3Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C21
	Cross = XMVector3Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C22
	Cross = XMVector3Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C23
	Cross = XMVector3Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C31
	Cross = XMVector3Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C32
	Cross = XMVector3Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// ������ : C33
	Cross = XMVector3Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&Interval), Cross)));
	if (L > rA + rB)
		return false;

	// �������ʂ����݂��Ȃ��̂Łu�Փ˂��Ă���v
	return true;
}

XMVECTOR COBB::GetDirect(int elem)  // �w�莲�ԍ��̕����x�N�g�����擾
{
	return m_NormaDirect[elem];
}
float COBB::GetLen_W(int elem)          // �w�莲�����̒������擾
{
	return m_fLength[elem];
}
XMFLOAT3 COBB::GetPos_W()             // �ʒu���擾
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
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
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
	// �ꎞ�z����
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

	CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		TEXTURE_FILENAME,		// �t�@�C���̖��O
	&g_pTextureBox);	// �ǂݍ��ރ������[

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
	

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
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
	// �ꎞ�z����
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
	// ���b�V���J��
	ReleaseMesh(pMesh);

	// �e�N�X�`���̊J��
	SAFE_RELEASE(g_pTextureBox);
}
void COBB::Update()
{	
	if (IsUpdated()) { return; }
	// ��ɍs��v�Z
	CObject::Update();

	// ---�I�u�W�F�N�g�̎p������e���ڂ̃Z�b�g--------------------------------------------------------------------
	XMFLOAT3 edgeposX, edgeposY, edgeposZ;
	XMFLOAT3 wepX, wepY, wepZ;
	float radX = XMConvertToRadians(GetWorldRotation().x), radY = XMConvertToRadians(GetWorldRotation().y), radZ = XMConvertToRadians(GetWorldRotation().z);

	// ---X���̒[���_���W�v�Z---
	edgeposX.x = GetWorldScale().x;
	edgeposX.y = 0.0f;
	edgeposX.z = 0.0f;
	// Z����]
	wepX.x = edgeposX.x * cosf(radZ) + edgeposX.y * sinf(radZ);
	wepX.y = edgeposX.x * -sinf(radZ) + edgeposX.y * cosf(radZ);
	wepX.z = edgeposX.z;
	edgeposX = wepX;

	// X����]
	wepX.x = edgeposX.x;
	wepX.y = edgeposX.y *cosf(radX) + edgeposX.z * sinf(radX);
	wepX.z = edgeposX.y * -sinf(radX) + edgeposX.z * cosf(radX);
	edgeposX = wepX;

	// Y����]
	wepX.x = edgeposX.x * cosf(radY) + edgeposX.z * -sinf(radY);
	wepX.y = edgeposX.y;
	wepX.z = edgeposX.x * sinf(radY) + edgeposX.z * cosf(radY);
	edgeposX = wepX;

	// ---Y���̒[���_���W�v�Z---
	edgeposY.x = 0.0f;
	edgeposY.y = GetWorldScale().y;
	edgeposY.z = 0.0f;
	// Z����]
	wepY.x = edgeposY.x * cosf(radZ) + edgeposY.y * sinf(radZ);
	wepY.y = edgeposY.x * -sinf(radZ) + edgeposY.y * cosf(radZ);
	wepY.z = edgeposY.z;
	edgeposY = wepY;

	// X����]
	wepY.x = edgeposY.x;
	wepY.y = edgeposY.y *cosf(radX) + edgeposY.z * sinf(radX);
	wepY.z = edgeposY.y * -sinf(radX) + edgeposY.z * cosf(radX);
	edgeposY = wepY;

	// Y����]
	wepY.x = edgeposY.x * cosf(radY) + edgeposY.z * -sinf(radY);
	wepY.y = edgeposY.y;
	wepY.z = edgeposY.x * sinf(radY) + edgeposY.z * cosf(radY);
	edgeposY = wepY;

	// ---Z���̒[���_���W�v�Z---
	edgeposZ.x = 0.0f;
	edgeposZ.y = 0.0f;
	edgeposZ.z = GetWorldScale().z;
	// Z����]
	wepZ.x = edgeposZ.x * cosf(radZ) + edgeposZ.y * sinf(radZ);
	wepZ.y = edgeposZ.x * -sinf(radZ) + edgeposZ.y * cosf(radZ);
	wepZ.z = edgeposZ.z;
	edgeposZ = wepZ;

	// X����]
	wepZ.x = edgeposZ.x;
	wepZ.y = edgeposZ.y *cosf(radX) + edgeposZ.z * sinf(radX);
	wepZ.z = edgeposZ.y * -sinf(radX) + edgeposZ.z * cosf(radX);
	edgeposZ = wepZ;

	// Y����]
	wepZ.x = edgeposZ.x * cosf(radY) + edgeposZ.z * -sinf(radY);
	wepZ.y = edgeposZ.y;
	wepZ.z = edgeposZ.x * sinf(radY) + edgeposZ.z * cosf(radY);
	edgeposZ = wepZ;


	// �x�N�g�����v�Z
	XMFLOAT3 vecX, vecY, vecZ;
	vecX = XMFLOAT3(edgeposX.x, edgeposX.y, edgeposX.z);
	vecY = XMFLOAT3(edgeposY.x, edgeposY.y, edgeposY.z);
	vecZ = XMFLOAT3(edgeposZ.x, edgeposZ.y, edgeposZ.z);
	// �P�ʃx�N�g�����i�[
	m_NormaDirect[0] = XMVector3Normalize(XMLoadFloat3(&vecX));
	m_NormaDirect[1] = XMVector3Normalize(XMLoadFloat3(&vecY));
	m_NormaDirect[2] = XMVector3Normalize(XMLoadFloat3(&vecZ));

	// �e�x�N�g���̒������i�[
	m_fLength[0] = sqrtf(vecX.x * vecX.x + vecX.y * vecX.y + vecX.z * vecX.z);
	m_fLength[1] = sqrtf(vecY.x * vecY.x + vecY.y * vecY.y + vecY.z * vecY.z);
	m_fLength[2] = sqrtf(vecZ.x * vecZ.x + vecZ.y * vecZ.y + vecZ.z * vecZ.z);

	// ------------------------------------------------------------------------------------------------------------


	
	
	
}
void COBB::Draw()
{
	CObject::Draw();	// �A�b�v�f�[�g�����t���O�����낷
	if (!m_bDispHitbox)return;
	if (!m_bUse)return;
	// ���݂̎p�������b�V���ɃZ�b�g
	m_pMesh->pos = GetWorldPosition();
	m_pMesh->rot = XMFLOAT3(XMConvertToRadians(GetWorldRotation().x), XMConvertToRadians(GetWorldRotation().y), XMConvertToRadians(GetWorldRotation().z));
	m_pMesh->size = GetWorldScale();
	UpdateMesh(m_pMesh);
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	GetCLight()->SetLightFalse();
	// ���b�V���`��
	DrawMesh(pDeviceContext, m_pMesh);
	GetCLight()->SetLightTrue();
}
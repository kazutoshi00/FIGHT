#include "CBillboard.h"

#include "Texture.h"
#include "debugproc.h"
#include "Light.h"
#include "Camera.h"

#define BILLBOARD_TEX_FILENAME L"data/texture/white.png"


//****************************************************************************
// メンバ関数実装
//****************************************************************************
HRESULT CBillboard::Init(const wchar_t* Filename,XMFLOAT2 size)
{
	HRESULT hr;

	ID3D11Device* pDevice = GetDevice();

	m_mesh.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_mesh.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_mesh.primitiveType = PT_UNDEFINED;
	XMStoreFloat4x4(&m_mesh.mtxWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mesh.mtxTexture, XMMatrixIdentity());

	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());

	m_size = size;

	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	

	MakeVertex(pDevice);

	hr = CreateTextureFromFile(pDevice, Filename, &m_mesh.pTexture);
	XMStoreFloat4x4(&m_mesh.mtxTexture, XMMatrixIdentity());

	m_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Power = 0.0f;
	m_mesh.pMaterial = &m_material;

	
	return hr;
}
HRESULT CBillboard::Init(XMFLOAT2 size)
{
	ID3D11Device* pDevice = GetDevice();

	m_mesh.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_mesh.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_mesh.primitiveType = PT_UNDEFINED;
	XMStoreFloat4x4(&m_mesh.mtxWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_mesh.mtxTexture, XMMatrixIdentity());

	XMStoreFloat4x4(&m_mtxWorld, XMMatrixIdentity());

	m_size = size;

	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_col = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	MakeVertex(pDevice);
	XMStoreFloat4x4(&m_mesh.mtxTexture, XMMatrixIdentity());

	m_material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_material.Power = 0.0f;
	m_mesh.pMaterial = &m_material;


	return S_OK;
}

void CBillboard::Uninit()
{
	ReleaseMesh(&m_mesh);
}
void CBillboard::Update()
{
	
}
void CBillboard::Draw()
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxTransrate, mtxRotate;
	XMFLOAT4X4 mtxView;

	GetCLight()->SetLightFalse();

	mtxView = CCamera::Get()->GetView();
	// ワールドマトリックスの初期化
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);

	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	mtxWorld = XMLoadFloat4x4(&m_mtxWorld);

	mtxScale = XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	mtxRotate = XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRotate);

	mtxTransrate = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTransrate);

	XMStoreFloat4x4(&m_mtxWorld, mtxWorld);
	m_mesh.mtxWorld = m_mtxWorld;

	m_material.Diffuse = m_col;
	DrawMesh(pDeviceContext, &m_mesh);


	GetCLight()->SetLightTrue();

}
HRESULT CBillboard::MakeVertex(ID3D11Device* pDevice)
{
	m_mesh.nNumVertex = 4;
	m_mesh.nNumIndex = 4;

	VERTEX_3D* pVertexWk = new VERTEX_3D[m_mesh.nNumVertex];

	VERTEX_3D* pVtx = pVertexWk;

	pVtx[0].vtx = XMFLOAT3(-m_size.x / 2, -m_size.y / 2, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-m_size.x / 2, m_size.y / 2, 0.0f);
	pVtx[2].vtx = XMFLOAT3(m_size.x / 2, -m_size.y / 2, 0.0f);
	pVtx[3].vtx = XMFLOAT3(m_size.x / 2, m_size.y / 2, 0.0f);

	pVtx[0].nrm = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nrm = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nrm = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nrm = XMFLOAT3(0.0f, 0.0f, -1.0f);

	pVtx[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	pVtx[0].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[1].tex = XMFLOAT2(1.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 1.0f);

	int* pIndexWk = new int[m_mesh.nNumIndex];

	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	HRESULT hr = MakeMeshVertex(pDevice, &m_mesh, pVertexWk, pIndexWk);

	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

void CBillboard::SetColor(XMFLOAT4 col)
{
	m_col = col;
}

void CBillboard::SetPos(XMFLOAT3 pos)
{
	m_pos = pos;
}

void CBillboard::SetSize(XMFLOAT2 size)
{
	m_size = size;
}
void CBillboard::SetScale(XMFLOAT3 scl)
{
	m_scl = scl;
}

void CBillboard::SetRot(XMFLOAT3 rot)
{
	m_rot = rot;
}
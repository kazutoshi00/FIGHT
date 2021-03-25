#include "BattleCamera.h"

#define InitPos1 (XMFLOAT3(-200.0f,0.0f,0.0f))
#define InitPos2 (XMFLOAT3(200.0f,0.0f,0.0f))

// 定数
namespace {
	const XMFLOAT3 g_vEye = XMFLOAT3(0.0f, 100.0f, -400.0f);
	const XMFLOAT3 g_vLook = XMFLOAT3(0.0f, 90.0f, 0.0f);
	const XMFLOAT3 g_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
};



CBattleCamera::CBattleCamera()
{
}


CBattleCamera::~CBattleCamera()
{
}

// 初期化
HRESULT CBattleCamera::Init()
{
	HRESULT hr = S_OK;
	m_vEye = g_vEye;
	m_vLook = g_vLook;
	m_vUp = g_vUp;
	m_fFOVY = XMConvertToRadians(45.0f);
	m_fAspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	m_fNearZ = 10.0f;
	m_fFarZ = 10000.0f;
	m_distZ = m_vEye.z - m_vLook.z;
	m_bIsSmallShake = false;
	m_bIsLargeShake = false;
	m_bPerspective = true;
	Update(InitPos1, InitPos2);
	CCamera::SetDirectPos();
	return hr;
}

// 終了処理
void CBattleCamera::Uninit()
{
	CCamera::Uninit();
}

// 更新
void CBattleCamera::Update(XMFLOAT3 pos1,XMFLOAT3 pos2)
{
	m_vEye.x = (pos1.x + pos2.x) / 2;
	m_vEye.z = fabsf(pos1.x - pos2.x) * -1;
	if (m_vEye.z >= -500.0f)m_vEye.z = -500.0f;
	m_vLook.x = m_vEye.x;
	Shake();
	m_vUp.x = g_vUp.x + m_shakeVal.x;
	m_vUp.z = g_vUp.z + m_shakeVal.z;
	// 視点と注視点を移動、上方ベクトルを回転
	XMMATRIX world = XMMatrixIdentity();
	XMStoreFloat3(&m_vEye, XMVector3TransformCoord(
		XMLoadFloat3(&m_vEye), world));
	XMStoreFloat3(&m_vLook, XMVector3TransformCoord(
		XMLoadFloat3(&m_vLook), world));
	XMStoreFloat3(&m_vUp, XMVector3TransformNormal(
		XMLoadFloat3(&m_vUp), world));
	// 行列更新
	CCamera::Update();
}

void CBattleCamera::SetShake(bool hit)
{
	if (hit)
	{
		if (!m_bIsLargeShake)
		{
			m_bIsLargeShake = true;
			m_shakecounter = 15;
			m_shakeVal = XMFLOAT3(0.2f, 0.0f, 0.2f);
		}
	}
	else
	{
		if (!m_bIsSmallShake)
		{
			m_bIsSmallShake = true;
			m_shakecounter = 7;
			m_shakeVal = XMFLOAT3(0.05f, 0.0f, 0.05f);
		}
	}
}

void CBattleCamera::Shake()
{
	if (m_shakecounter < 0) 
	{
		m_bIsSmallShake = false;
		m_bIsLargeShake = false;
		m_shakeVal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		return;
	}
	m_shakecounter--;
	m_shakeVal.x *= -1;
	m_shakeVal.z *= -1;
}

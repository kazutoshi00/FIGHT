// カメラ [Camera.cpp]
#include "Camera.h"

// 静的メンバ
CCamera* CCamera::m_pCamera = nullptr;
XMFLOAT3 CCamera::m_vNowEye;	// 現在の視点
XMFLOAT3 CCamera::m_vNowLook;	// 現在の注視点
XMFLOAT3 CCamera::m_vNowUp;		// 現在の上方ベクトル



// 初期化
HRESULT CCamera::Init()
{
	HRESULT hr = S_OK;
	m_vEye = XMFLOAT3(0.0f, 100.0f, 400.0f);
	m_vLook = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fFOVY = XMConvertToRadians(45);
	m_fAspect = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	m_fNearZ = 10.0f;
	m_fFarZ = 10000.0f;
	m_vNowEye = m_vEye;
	m_vNowLook = m_vLook;
	m_vNowUp = m_vUp;
	m_bPerspective = true;	
	Update();
	return hr;
}

// 終了処理
void CCamera::Uninit()
{
}

// 更新
void CCamera::Update()
{
	// 視点、注視点、上方ベクトルを近づける
	m_vNowEye.x = m_vNowEye.x * 0.8f + m_vEye.x * 0.2f;
	m_vNowEye.y = m_vNowEye.y * 0.8f + m_vEye.y * 0.2f;
	m_vNowEye.z = m_vNowEye.z * 0.8f + m_vEye.z * 0.2f;
	m_vNowLook.x = m_vNowLook.x * 0.8f + m_vLook.x * 0.2f;
	m_vNowLook.y = m_vNowLook.y * 0.8f + m_vLook.y * 0.2f;
	m_vNowLook.z = m_vNowLook.z * 0.8f + m_vLook.z * 0.2f;
	m_vNowUp.x = m_vNowUp.x * 0.8f + m_vUp.x * 0.2f;
	m_vNowUp.y = m_vNowUp.y * 0.8f + m_vUp.y * 0.2f;
	m_vNowUp.z = m_vNowUp.z * 0.8f + m_vUp.z * 0.2f;
	XMStoreFloat3(&m_vNowUp,
		XMVector3Normalize(XMLoadFloat3(&m_vNowUp)));
	// ビュー変換更新
	XMStoreFloat4x4(&m_mView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vNowEye),
		XMLoadFloat3(&m_vNowLook),
		XMLoadFloat3(&m_vNowUp)));


	// 射影変換更新
	if (m_bPerspective)
	{
		// 透視投影
		XMStoreFloat4x4(&m_mProj,
			XMMatrixPerspectiveFovLH(m_fFOVY, m_fAspect,
				m_fNearZ, m_fFarZ));
	}
	else
	{
		// 平行投影
		float nearH = 500, nearW = 500;
		nearH = m_fNearZ * tanf(m_fFOVY / 2) * 2;
		nearW = nearH * m_fAspect;
		XMStoreFloat4x4(&m_mProj,
			XMMatrixOrthographicLH(nearW, nearH, m_fNearZ, m_fFarZ));
	}
}
void CCamera::SetDirectPos()
{
	// 視点、注視点、上方ベクトルを近づける
	m_vNowEye.x =m_vEye.x;
	m_vNowEye.y =m_vEye.y;
	m_vNowEye.z =m_vEye.z;
	m_vNowLook.x =m_vLook.x;
	m_vNowLook.y =m_vLook.y;
	m_vNowLook.z =m_vLook.z;
	m_vNowUp.x =m_vUp.x;
	m_vNowUp.y =m_vUp.y;
	m_vNowUp.z =m_vUp.z;
	XMStoreFloat3(&m_vNowUp,
		XMVector3Normalize(XMLoadFloat3(&m_vNowUp)));
	// ビュー変換更新
	XMStoreFloat4x4(&m_mView, XMMatrixLookAtLH(
		XMLoadFloat3(&m_vNowEye),
		XMLoadFloat3(&m_vNowLook),
		XMLoadFloat3(&m_vNowUp)));


	// 射影変換更新
	if (m_bPerspective)
	{
		// 透視投影
		XMStoreFloat4x4(&m_mProj,
			XMMatrixPerspectiveFovLH(m_fFOVY, m_fAspect,
				m_fNearZ, m_fFarZ));
	}
	else
	{
		// 平行投影
		float nearH = 500, nearW = 500;
		nearH = m_fNearZ * tanf(m_fFOVY / 2) * 2;
		nearW = nearH * m_fAspect;
		XMStoreFloat4x4(&m_mProj,
			XMMatrixOrthographicLH(nearW, nearH, m_fNearZ, m_fFarZ));
	}
}

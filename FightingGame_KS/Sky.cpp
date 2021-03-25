// スカイドーム/スカイボックス [Sky.cpp]
#include "Sky.h"
#include "Camera.h"

// マクロ定義
#define PATH_MODELSKY	"data/model/sky.fbx"

// コンストラクタ
CSky::CSky() : m_pModel(nullptr)
{
	// 光源無効化
	m_lightOff.m_direction = XMFLOAT3(0.f, 0.f, 0.f);
}

// デストラクタ
CSky::~CSky()
{
	SAFE_DELETE(m_pModel);
}

// 初期化
HRESULT CSky::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// FBXファイルの読み込み
	SAFE_DELETE(m_pModel);
	m_pModel = new CFbxModel();
	hr = m_pModel->Init(pDevice, pDeviceContext, PATH_MODELSKY);
	if (SUCCEEDED(hr)) {
		m_pModel->SetCamera(CCamera::Get()->GetEye());
		m_pModel->SetLight(m_lightOff);
	}
	return hr;
}

// 終了処理
void CSky::Uninit()
{
	// モデル削除
	SAFE_DELETE(m_pModel);
}

// 更新
void CSky::Update()
{
	// 何もしない.
}

// 描画
void CSky::Draw()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	// FBXファイル表示
	SetZWrite(false);				// Zバッファ更新しない
	SetBlendState(BS_NONE);			// アルファ処理しない
	XMFLOAT4X4 world;
	CCamera* pCamera = CCamera::Get();
	XMFLOAT3& vEye = pCamera->GetEye();
	XMStoreFloat4x4(&world,
		XMMatrixTranslation(vEye.x, vEye.y, vEye.z));
	m_pModel->Render(world, pCamera->GetView(),
		pCamera->GetProj(), eOpacityOnly);
	SetBlendState(BS_ALPHABLEND);	// 半透明描画
	SetZWrite(true);				// Zバッファ更新する
}

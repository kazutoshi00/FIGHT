// �X�J�C�h�[��/�X�J�C�{�b�N�X [Sky.cpp]
#include "Sky.h"
#include "Camera.h"

// �}�N����`
#define PATH_MODELSKY	"data/model/sky.fbx"

// �R���X�g���N�^
CSky::CSky() : m_pModel(nullptr)
{
	// ����������
	m_lightOff.m_direction = XMFLOAT3(0.f, 0.f, 0.f);
}

// �f�X�g���N�^
CSky::~CSky()
{
	SAFE_DELETE(m_pModel);
}

// ������
HRESULT CSky::Init()
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// FBX�t�@�C���̓ǂݍ���
	SAFE_DELETE(m_pModel);
	m_pModel = new CFbxModel();
	hr = m_pModel->Init(pDevice, pDeviceContext, PATH_MODELSKY);
	if (SUCCEEDED(hr)) {
		m_pModel->SetCamera(CCamera::Get()->GetEye());
		m_pModel->SetLight(m_lightOff);
	}
	return hr;
}

// �I������
void CSky::Uninit()
{
	// ���f���폜
	SAFE_DELETE(m_pModel);
}

// �X�V
void CSky::Update()
{
	// �������Ȃ�.
}

// �`��
void CSky::Draw()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	// FBX�t�@�C���\��
	SetZWrite(false);				// Z�o�b�t�@�X�V���Ȃ�
	SetBlendState(BS_NONE);			// �A���t�@�������Ȃ�
	XMFLOAT4X4 world;
	CCamera* pCamera = CCamera::Get();
	XMFLOAT3& vEye = pCamera->GetEye();
	XMStoreFloat4x4(&world,
		XMMatrixTranslation(vEye.x, vEye.y, vEye.z));
	m_pModel->Render(world, pCamera->GetView(),
		pCamera->GetProj(), eOpacityOnly);
	SetBlendState(BS_ALPHABLEND);	// �������`��
	SetZWrite(true);				// Z�o�b�t�@�X�V����
}

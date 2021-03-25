// 自機クラス [Player.cpp]
#include "CharacterEric.h"
#include "input.h"
#include "Camera.h"
#include "debugproc.h"
#include "Light.h"
#include "Texture.h"
#include "SceneManager.h"

// マクロ定義
#define PATH_PLAYER		"data/model/CharacterModel.fbx"
#define ICONTEXTURE		L"data/texture/Playericon.png"
#define HITEFFECTTEXTURE L"data/texture/HitEffect.png"
#define GUARDEFFECTTEXTURE L"data/texture/GuardEffect.png"
#define FSPEED			(3.0f)	// 速さ
#define BSPEED			(2.0f)	// 速さ
#define INITPOS XMFLOAT3(-200.0f,0.0f,0.0f);
#define INITROT XMFLOAT3(0.0f,90.0f,0.0f);

//

const XMFLOAT3 g_BodyHitBoxData[Character_Eric::MAX_ANM][3] = {
	// pos					rot				size
	{{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},				// Tpose(使わない)
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			// Idle
	{{1.0f,86.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,87.0f,40.0f}},			// Walk
	{{-10.0f,81.0f,26.0f},{0.0f,0.0f,0.0f},{41.0f,84.0f,46.0f}},		// Punch
	{{-6.0f,81.0f,26.0f},{0.0f,0.0f,0.0f},{35.0f,77.0f,39.0f}},			// Elbow
	{{0.0f,85.0f,25.0f},{0.0f,0.0f,0.0f},{45.0f,84.0f,53.0f}},			// Headbutt
	{{0.0f,84.0f,9.0f},{0.0f,0.0f,0.0f},{61.0f,80.0f,45.0f}},			// Kick
	{{0.0f,84.0f,9.0f},{0.0f,0.0f,0.0f},{61.0f,80.0f,45.0f}},			// HighKick
	{{-4.0f,83.0f,8.0f},{0.0f,0.0f,0.0f},{38.0f,82.0f,45.0f}},			// Knee
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
	{{10.0f,80.0f,0.0f},{0.0f,0.0f,0.0f},{39.0f,79.0f,40.0f}},			//
};

const XMFLOAT3 g_AttackHitBoxData[Character_Eric::ATK_MAX][3] = {
	{{11.0f,107.0f,109.0f},{0.0f,0.0f,0.0f},{16.0f,14.0f,33.0f}},		// Punch
	{{11.0f,132.0f,61.0f},{0.0f,0.0f,0.0f},{24.0f,14.0f,26.0f}},		// Elbow
	{{0.0f,139.0f,69.0f},{0.0f,0.0f,0.0f},{25.0f,24.0f,43.0f}},			// Headbutt
	{{0.0f,103.0f,101.0f},{0.0f,0.0f,0.0f},{25.0f,24.0f,58.0f}},		// Kick
	{{-10.0f,135.0f,90.0f},{-40.0f,0.0f,0.0f},{41.0f,17.0f,42.0f}},		// HighKick
	{{-8.0f,97.0f,64.0f},{0.0f,0.0f,0.0f},{31.0f,36.0f,22.0f}},			// Knee
	{{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}},				// NONE
};

const AttackData c_AttackData[6] = {
	//全体	発生	持続	硬直	相手硬直(ヒット)	相手硬直(ガード)	ヒットストップ	攻撃力	ヒットバック	ガードバック	使用SE
	{20,	6,		5,		9,		15,					7,					4,				5.0f,	30.0f,			40.0f,			SOUND_LABEL_SE_Punch},	// Punch
	{15,	5,		3,		7,		15,					7,					2,				3.0f,	20.0f,			30.0f,			SOUND_LABEL_SE_Punch},	// Elbow
	{30,	9,		6,		15,		15,					7,					15,				10.0f,	150.0f,			60.0f,			SOUND_LABEL_SE_Punch},	// Headbutt
	{20,	6,		5,		9,		15,					7,					4,				5.0f,	30.0f,			40.0f,			SOUND_LABEL_SE_Kick},	// Kick
	{30,	9,		6,		15,		15,					7,					15,				7.0f,	150.0f,			60.0f,			SOUND_LABEL_SE_Kick},	// HighKick
	{15,	5,		3,		7,		15,					7,					2,				5.0f,	20.0f,			30.0f,			SOUND_LABEL_SE_Kick},	// Knee
};
const AttackAnimation c_AttackAnimation[6] = {
	//全体フレーム	番号	発生	持続	硬直	
	{{101,			3},		21,		11,		69},	// Punch
	{{100,			4},		38,		11,		51},	// Elbow
	{{120,			5},		34,		16,		70},	// Headbutt
	{{83,			6},		36,		7,		40},	// Kick
	{{91,			7},		42,		5,		44},	// HighKick
	{{106,			8},		35,		16,		57},	// Knee
};

const int g_AttackFrame[6] = { 223,341,455,560,648,737 };

// コンストラクタ
CCharacterEric::CCharacterEric(bool bCPU) : m_pModel(nullptr)
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
	m_bHit = false;
	m_bCPUPlay = bCPU;
}

// デストラクタ
CCharacterEric::~CCharacterEric()
{
	SAFE_DELETE(m_pModel);
}

void CCharacterEric::Reset()
{
	m_localposition = INITPOS;
	m_localrotation = INITROT;
	m_bDispHitbox = false;
	m_nAnmFrameCount = 0;

	m_icon.SetPos(XMFLOAT3(m_localposition.x, m_localposition.y + 200.0f, m_localposition.z));

	m_pBodyHitBox->SetUse(true);

	m_State = Character_Eric::PS_IDLE;
	m_fHp = m_fMaxHp;
	m_fHitBackThisFrame = 0;
	m_nThinkCount = 0;
	m_pNowAttackDataDetails = nullptr;
	// 初期アニメーション
	ChangeAnimation(Character_Eric::ANM_Idle);
}

// 初期化
HRESULT CCharacterEric::Init()
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	HRESULT hr = S_OK;
	// FBXファイルの読み込み
	SAFE_DELETE(m_pModel);
	m_pModel = new CFbxModel();
	hr = m_pModel->Init(pDevice, pDeviceContext, PATH_PLAYER);
	if (SUCCEEDED(hr)) {
		m_pModel->SetCamera(CCamera::Get()->GetEye());
		m_pModel->SetLight(GetCLight()->GetLight());

		
/*{
	TCHAR szMsg[256];
	_stprintf_s(szMsg, 256, _T("m_vBBox={%f, %f, %f}"),
m_vBBox.x, m_vBBox.y, m_vBBox.z);
	MessageBox(GetMainWnd(), szMsg, _T("確認"), MB_OK);
}*/

	}
	m_icon.Init(ICONTEXTURE,XMFLOAT2(50.0f,30.0f));
	m_HitEffect.Init(HITEFFECTTEXTURE, XMFLOAT2(100.0f,100.0f));
	m_GuardEffect.Init(GUARDEFFECTTEXTURE, XMFLOAT2(100.0f, 100.0f));

	bool m_bUseGuardEffect = false;
	bool m_bUseHitEffect = false;

	m_localposition = INITPOS;
	m_localrotation = INITROT;
	m_bDispHitbox = false;
	m_nAnmFrameCount = 0;

	m_pAttackHitBox = new COBB;
	m_pAttackHitBox->Init();
	m_pAttackHitBox->SetSize(XMFLOAT3(100.0f, 100.0f, 100.0f));
	m_pAttackHitBox->SetPos(XMFLOAT3(0.0f, 50.0f, 0.0f));
	m_pAttackHitBox->SetParent(this);
	CObjectManager::Get()->AddObject(m_pAttackHitBox);
	m_pAttackHitBox->SetUse(false);

	m_pBodyHitBox = new COBB;
	m_pBodyHitBox->Init();
	m_pBodyHitBox->SetSize(XMFLOAT3(100.0f, 100.0f, 100.0f));
	m_pBodyHitBox->SetPos(XMFLOAT3(0.0f, 50.0f, 0.0f));
	m_pBodyHitBox->SetParent(this);
	CObjectManager::Get()->AddObject(m_pBodyHitBox);
	m_pBodyHitBox->SetUse(true);

	// ---攻撃の設定
	m_AttackDataDetailsList.resize(6);	// 攻撃の個数分配列をとる
	for (int i = 0; i < m_AttackDataDetailsList.size(); i++)
	{
		// メモリ確保 
		m_AttackDataDetailsList[i].pAttackAnimaion = new AttackAnimation;
		m_AttackDataDetailsList[i].pAttackData = new AttackData;
		// 値設定
		m_AttackDataDetailsList[i].pAttackData->AtacckDamage = c_AttackData[i].AtacckDamage;
		m_AttackDataDetailsList[i].pAttackData->HitStopFrame = c_AttackData[i].HitStopFrame;
		m_AttackDataDetailsList[i].pAttackData->InflictGuardBackDistance = c_AttackData[i].InflictGuardBackDistance;
		m_AttackDataDetailsList[i].pAttackData->InflictGuardRigidityFrame = c_AttackData[i].InflictGuardRigidityFrame;
		m_AttackDataDetailsList[i].pAttackData->InflictHitBackDistance = c_AttackData[i].InflictHitBackDistance;
		m_AttackDataDetailsList[i].pAttackData->InflictHitRigidityFrame = c_AttackData[i].InflictHitRigidityFrame;
		m_AttackDataDetailsList[i].pAttackData->OccurrenceFrame = c_AttackData[i].OccurrenceFrame;
		m_AttackDataDetailsList[i].pAttackData->OverallFrame = c_AttackData[i].OverallFrame;
		m_AttackDataDetailsList[i].pAttackData->RigidityFrame = c_AttackData[i].RigidityFrame;
		m_AttackDataDetailsList[i].pAttackData->SustainFrame = c_AttackData[i].SustainFrame;
		m_AttackDataDetailsList[i].pAttackData->useSE = c_AttackData[i].useSE;
		// アニメーション関連の値設定
		m_AttackDataDetailsList[i].pAttackAnimaion->AnimationData.AnimationNumber = c_AttackAnimation[i].AnimationData.AnimationNumber;
		m_AttackDataDetailsList[i].pAttackAnimaion->AnimationData.OverallFrame = c_AttackAnimation[i].AnimationData.OverallFrame;
		m_AttackDataDetailsList[i].pAttackAnimaion->OccurrenceFrame = c_AttackAnimation[i].OccurrenceFrame;
		m_AttackDataDetailsList[i].pAttackAnimaion->RigidityFrame = c_AttackAnimation[i].RigidityFrame;
		m_AttackDataDetailsList[i].pAttackAnimaion->SustainFrame = c_AttackAnimation[i].SustainFrame;
	}
	// コマンドの設定
	bool usecom[COMMAND_MAX] = { true,true,false,false,false,false,false,false };
	m_pInputPreservation = new CInputPreservation(usecom);

	m_State = Character_Eric::PS_IDLE;
	m_nowAttack = Character_Eric::ATK_NONE;
	m_fHp = m_fMaxHp = 200.0f;
	m_fHitBackThisFrame = 0.0f;
	m_nThinkCount = 0;
	m_nRigidityFrame = 0;
	m_pNowAttackDataDetails = nullptr;
	// 初期アニメーション
	ChangeAnimation(Character_Eric::ANM_Idle);


	return hr;
}

// 終了処理
void CCharacterEric::Uninit()
{
	CObjectManager::Get()->DeleteObject(m_pBodyHitBox);
	m_pBodyHitBox->Uninit();
	SAFE_DELETE(m_pBodyHitBox);
	CObjectManager::Get()->DeleteObject(m_pAttackHitBox);
	m_pAttackHitBox->Uninit();
	SAFE_DELETE(m_pAttackHitBox);
	m_icon.Uninit();
	m_HitEffect.Uninit();
	m_GuardEffect.Uninit();
	// FBXモデルの解放
	SAFE_DELETE(m_pModel);
}

// 更新
void CCharacterEric::Update()
{
	if (IsUpdated()) { return; }
	m_bHit = false;
	m_bUseGuardEffect = false;
	m_bUseHitEffect = false;

	if (m_nRigidityFrame > 0)
	{
		if (m_State == Character_Eric::PS_DAMAGE || m_State == Character_Eric::PS_GUARD)
		{
			m_localposition.x += m_fHitBackThisFrame;
			m_fHitBackThisFrame /= HitBackDecPar;
		}
		m_nRigidityFrame--;
		if (m_nRigidityFrame <= 0 && m_State != Character_Eric::PS_ATTACK)
		{
			m_State = Character_Eric::PS_IDLE;
			ChangeAnimation(Character_Eric::ANM_Idle);
			m_fHitBackThisFrame = 0.0f;
		}
		
	}
	else
	{
		if (m_bCPUPlay)
		{
			CPUUpdate();
		}
		else
		{
			PlayerUpdate();
		}
	}

	// -------アニメーション管理
	
	if (m_State == Character_Eric::PS_ATTACK)
	{
		if (m_fHitBackThisFrame != 0.0f)
		{
			m_localposition.x += m_fHitBackThisFrame;
			m_fHitBackThisFrame /= HitBackDecPar;
		}
		m_nAttackFrame++;
		if (m_nAttackFrame > m_pNowAttackDataDetails->pAttackData->OverallFrame)
		{	// 攻撃終わり
			m_nAnmFrameCount = m_pModel->GetMaxAnimFrame();
			SetEndAttackStatus();
		}
		else if (m_nAttackFrame <= m_pNowAttackDataDetails->pAttackData->OccurrenceFrame)
		{	// 発生中
			m_nAnmFrameCount = m_pModel->GetMinAnimFrame() + (m_nAttackFrame * (m_pNowAttackDataDetails->pAttackAnimaion->OccurrenceFrame / (m_pNowAttackDataDetails->pAttackData->OccurrenceFrame + 1)));
		}
		else if (m_nAttackFrame <= m_pNowAttackDataDetails->pAttackData->OccurrenceFrame + m_pNowAttackDataDetails->pAttackData->SustainFrame)
		{	// 持続中
			m_nAnmFrameCount = m_pModel->GetMinAnimFrame() + m_pNowAttackDataDetails->pAttackAnimaion->OccurrenceFrame + (m_nAttackFrame - m_pNowAttackDataDetails->pAttackData->OccurrenceFrame) * (m_pNowAttackDataDetails->pAttackAnimaion->SustainFrame / (m_pNowAttackDataDetails->pAttackData->SustainFrame + 1));
			// 攻撃ヒットボックスが子の攻撃中に1度もアクティブになってなかったら、アクティブにする
			if (!m_bAlreadyActiveAttackHitBox)
			{
				m_pAttackHitBox->SetUse(true);
				m_bAlreadyActiveAttackHitBox = true;
			}
		}
		else
		{	// 硬直中
			m_nAnmFrameCount = m_pModel->GetMinAnimFrame() + m_pNowAttackDataDetails->pAttackAnimaion->OccurrenceFrame + m_pNowAttackDataDetails->pAttackAnimaion->SustainFrame + (m_nAttackFrame - m_pNowAttackDataDetails->pAttackData->OccurrenceFrame - m_pNowAttackDataDetails->pAttackData->SustainFrame) * (m_pNowAttackDataDetails->pAttackAnimaion->RigidityFrame / (m_pNowAttackDataDetails->pAttackData->RigidityFrame + 1));
			// 攻撃ヒットボックスを無効化
			m_pAttackHitBox->SetUse(false);
		}
	}
	else if (m_State == Character_Eric::PS_DAMAGE)
	{
		int anmFrameLength = m_pModel->GetMaxAnimFrame() - m_pModel->GetMinAnimFrame();
		m_nAnmFrameCount += anmFrameLength / m_nReceiveRigidityFrame;
	}
	else if (m_State == Character_Eric::PS_WALK)
	{
		m_nAnmFrameCount += 2;
	}
	else if(m_State != Character_Eric::PS_BACK)
	{
		m_nAnmFrameCount++;
	}
	if (m_State == Character_Eric::PS_BACK)
	{
		m_nAnmFrameCount --;
		if (m_nAnmFrameCount <= m_pModel->GetMinAnimFrame())
			m_nAnmFrameCount = m_pModel->GetMaxAnimFrame();
	}
	else if (m_nAnmFrameCount >= m_pModel->GetMaxAnimFrame())
	{
		switch (m_NowAnimation)
		{
		case Character_Eric::ANM_Tpose:
			break;
		case Character_Eric::ANM_Idle:
			ChangeAnimation(Character_Eric::ANM_Idle);
			break;
		case Character_Eric::ANM_Walk:
			ChangeAnimation(Character_Eric::ANM_Walk);
			break;
		/*case Character_Eric::ANM_Punch:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_ElbowPunch:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_Headbutt:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_Kick:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_HighKick:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_Knee:
			ChangeAnimation(Character_Eric::ANM_Idle);
			if (m_State != Character_Eric::PS_WIN && m_State != Character_Eric::PS_LOSE)
				m_State = Character_Eric::PS_IDLE;
			break;*/
		case Character_Eric::ANM_HeadDamageSmall:
			ChangeAnimation(Character_Eric::ANM_Idle);
			m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_HeadDamageLarge:
			ChangeAnimation(Character_Eric::ANM_Idle);
			m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_KickDamage:
			ChangeAnimation(Character_Eric::ANM_Idle);
			m_State = Character_Eric::PS_IDLE;
			break;
		case Character_Eric::ANM_DownForPunch:
			ChangeScene(RESULT);
			m_nAnmFrameCount--;
			break;
		case Character_Eric::ANM_DownForKick:
			ChangeScene(RESULT);
			m_nAnmFrameCount--;
			break;
		case Character_Eric::ANM_IdleToPunch:
			ChangeAnimation(Character_Eric::ANM_Punch);
			break;
		case Character_Eric::ANM_IdleToKick:
			ChangeAnimation(Character_Eric::ANM_Kick);
			break;
		case Character_Eric::ANM_IdleToHeadbutt:
			ChangeAnimation(Character_Eric::ANM_Headbutt);
			break;
		case Character_Eric::ANM_IdleToHighKick:
			ChangeAnimation(Character_Eric::ANM_HighKick);
			break;
		default: break;
		}

	}
	
	/*if (ColObbs(m_AttackHitBox, m_testobb))
	PrintDebugProc("Hit\n");
	else
		PrintDebugProc("Unhit\n");*/
	
	// カメラ座標を反映
	m_pModel->SetCamera(CCamera::Get()->GetEye());
	// 光源上方を反映
	m_pModel->SetLight(GetCLight()->GetLight());

	CObject::Update();	// ワールドマトリックス計算(親のアップデート)

	m_pBodyHitBox->Update();
	m_pAttackHitBox->Update();

	m_icon.SetPos(XMFLOAT3(m_localposition.x, m_localposition.y + 200.0f, m_localposition.z));

	XMFLOAT3 camposwork;
	if (GetKeyPress(VK_UP))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.z += 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}
	if (GetKeyPress(VK_DOWN))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.z -= 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}
	if (GetKeyPress(VK_RIGHT))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.x += 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}
	if (GetKeyPress(VK_LEFT))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.x -= 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}
	if (GetKeyPress(VK_RCONTROL))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.y -= 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}
	if (GetKeyPress(VK_RSHIFT))
	{
		camposwork = CCamera::Get()->GetEye();
		camposwork.y += 3.0f;
		CCamera::Get()->SetEye(camposwork);
	}


	PrintDebugProc("Frame:[%d]\n", m_nAnmFrameCount);

	/*PrintDebugProc("[ﾎｳｺｳｷｰ:ｳｴ]  :Down\n");
	PrintDebugProc("[ﾎｳｺｳｷｰ:ｼﾀ]  :Up\n");
	PrintDebugProc("[ﾎｳｺｳｷｰ:ﾐｷﾞ] :TurningRight\n");
	PrintDebugProc("[ﾎｳｺｳｷｰ:ﾋﾀﾞﾘ]:TurningLeft\n");
	PrintDebugProc("[SPACE]      :Missilelaunch\n");
	PrintDebugProc("[P]:DispHitBox\n");
	PrintDebugProc("[1]:FPCAM\n");
	PrintDebugProc("[2]:BTCAM\n");
	PrintDebugProc("[3]:TPCAM\n");
	PrintDebugProc("[0]:FixedPosCAM\n");*/
}

// 描画
void CCharacterEric::Draw()
{
	CObject::Draw();	// アップデート完了フラグをおろす
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	CCamera* pCamera = CCamera::Get();

	// アニメーションフレームを設定
	m_pModel->SetAnimFrame(m_nAnmFrameCount);


	//XMMATRIX mtxWorld, mtxRot, mtxTranslate;

	//// ワールドマトリックスの初期化
	//mtxWorld = XMMatrixIdentity();

	//// 回転を反映
	//mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_localrotation.x), XMConvertToRadians(m_localrotation.y), XMConvertToRadians(m_localrotation.z));
	//mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//// 移動を反映
	//mtxTranslate = XMMatrixTranslation(m_localposition.x, m_localposition.y, m_localposition.z);
	//mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//// ワールドマトリックスの設定
	//XMStoreFloat4x4(&m_world, mtxWorld);

	// FBXファイル表示
	SetBlendState(BS_NONE);			// アルファ処理しない
	m_pModel->Render(m_world, pCamera->GetView(),
		pCamera->GetProj(), eOpacityOnly);
	SetZWrite(false);
	SetBlendState(BS_ALPHABLEND);	// 半透明描画
	m_pModel->Render(m_world, pCamera->GetView(),
		pCamera->GetProj(), eTransparentOnly);

	// ヒットボックスを描画するかどうかの設定
	m_pAttackHitBox->SetDisp(m_bDispHitbox);
	m_pBodyHitBox->SetDisp(m_bDispHitbox);

	SetCullMode(CULLMODE_CCW);	// 背面カリング(裏を描かない)
	//m_icon.Draw();
	if (m_bUseHitEffect)
	{
		m_HitEffect.SetColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		m_HitEffect.Draw();
	}
	if (m_bUseGuardEffect)
	{
		m_HitEffect.SetColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		m_GuardEffect.Draw();
	}
	SetCullMode(CULLMODE_CW);	// 前面カリング(表を描かない)
	SetZWrite(true);
}
void CCharacterEric::DebugDraw()
{
	m_pInputPreservation->Draw();
}
void CCharacterEric::SetDrawDeb(bool flg)
{
	m_bDispHitbox = flg;
}
// 衝突有無
void CCharacterEric::SetHit(bool bHit)
{
	m_bHit = bHit;
}

void CCharacterEric::ChangeAnimation(Character_Eric::ANIMATION_KIND anm)
{
	m_NowAnimation = anm;
	m_pModel->SetAnimStack(anm);
	m_nAnmFrameCount = m_pModel->GetMinAnimFrame();
	m_pBodyHitBox->SetPos(g_BodyHitBoxData[anm][0]);
	m_pBodyHitBox->SetRot(g_BodyHitBoxData[anm][1]);
	m_pBodyHitBox->SetSize(g_BodyHitBoxData[anm][2]);

	m_pAttackHitBox->SetPos(g_AttackHitBoxData[m_nowAttack][0]);
	m_pAttackHitBox->SetRot(g_AttackHitBoxData[m_nowAttack][1]);
	m_pAttackHitBox->SetSize(g_AttackHitBoxData[m_nowAttack][2]);
}

//****************************************************
// キャラクター操作時のアップデート
//****************************************************
void CCharacterEric::PlayerUpdate()
{
	//if (m_State == Character_Eric::PS_WALK || m_State == Character_Eric::PS_IDLE || m_State == Character_Eric::PS_BACK)
	{
		bool bAction = false;
		const char* UseKey = m_pInputPreservation->GetUseKey();

		// 現在有効になっている入力コマンドによって処理をする
		switch (m_pInputPreservation->GetCommand())
		{
			case CommandKind::COMMAND_HADOU:
				if ((GetKeyTrigger(UseKey[UB_Punch1])|| GetKeyTrigger(UseKey[UB_Punch2])) && !bAction) {
					m_nowAttack = Character_Eric::ATK_HEADBUTT;
					ChangeAnimation(Character_Eric::ANM_Headbutt);
					SetAttackStatus();
					bAction = true;
				}
				else if ((GetKeyTrigger(UseKey[UB_Kick1]) || GetKeyTrigger(UseKey[UB_Kick2])) && !bAction) {
					m_nowAttack = Character_Eric::ATK_HIGHKICK;
					ChangeAnimation(Character_Eric::ANM_HighKick);
					SetAttackStatus();
					bAction = true;
				}
				break;
			case CommandKind::COMMAND_REVHADOU:
				
				break;
			default:
				break;
		}
		// PUNCH
		if (GetKeyTrigger(UseKey[UB_Punch1])&&!bAction) {
			m_nowAttack = Character_Eric::ATK_PUNCH;
			ChangeAnimation(Character_Eric::ANM_Punch);
			SetAttackStatus();
			bAction = true;
		}
		// ELBOW
		if (GetKeyTrigger(UseKey[UB_Punch2]) && !bAction) {
			m_nowAttack = Character_Eric::ATK_ELBOW;
			ChangeAnimation(Character_Eric::ANM_ElbowPunch);
			SetAttackStatus();
			bAction = true;
		}
		// KICK
		if (GetKeyTrigger(UseKey[UB_Kick1]) && !bAction) {
			m_nowAttack = Character_Eric::ATK_MIDDLEKICK;
			ChangeAnimation(Character_Eric::ANM_IdleToKick);
			SetAttackStatus();
			bAction = true;
		}
		// Knee
		if (GetKeyTrigger(UseKey[UB_Kick2]) && !bAction) {
			m_nowAttack = Character_Eric::ATK_KNEE;
			ChangeAnimation(Character_Eric::ANM_Knee);
			SetAttackStatus();
			bAction = true;
		}


		if (GetKeyPress(UseKey[UB_Left]) && !bAction)
		{
			m_localposition.x -= BSPEED;
			if (m_localposition.x <= LimitMinPos)
			{
				m_localposition.x = LimitMinPos;
			}
			if (m_State != Character_Eric::PS_BACK)
			{
				m_State = Character_Eric::PS_BACK;
				ChangeAnimation(Character_Eric::ANM_Walk);
			}
			bAction = true;
		}
		if (GetKeyPress(UseKey[UB_Right]) && !bAction)
		{
			m_localposition.x += FSPEED;
			if (m_localposition.x >= LimitMaxPos)
			{
				m_localposition.x = LimitMaxPos;
			}
			if (m_State != Character_Eric::PS_WALK)
			{
				m_State = Character_Eric::PS_WALK;
				ChangeAnimation(Character_Eric::ANM_Walk);
			}
		}
		if (!GetKeyPress(UseKey[UB_Right]) && !GetKeyPress(UseKey[UB_Left]) && !bAction)
		{
			if (m_State != Character_Eric::PS_IDLE)
			{
				m_State = Character_Eric::PS_IDLE;
				ChangeAnimation(Character_Eric::ANM_Idle);
			}
		}
	}
}

//*****************************************************
// キャラクターCPU時のアップデート(敵AI)
//*****************************************************
void CCharacterEric::CPUUpdate()
{

	if (m_State == Character_Eric::PS_WALK || m_State == Character_Eric::PS_IDLE || m_State == Character_Eric::PS_BACK)
	{
		float distPE = abs(m_pOpponent->GetPos().x - m_localposition.x);
		if (m_nThinkCount > (100 - (60 - distPE / 10)))CPUAIthinkUpdate();
		bool bAction = false;
		switch (m_nowAttack)
		{
			// PUNCH
		case Character_Eric::ATK_PUNCH:
			m_nowAttack = Character_Eric::ATK_PUNCH;
			ChangeAnimation(Character_Eric::ANM_Punch);
			SetAttackStatus();
			bAction = true;
				break;
			// ELBOW
		case Character_Eric::ATK_ELBOW:
			if (bAction)break;
			m_nowAttack = Character_Eric::ATK_ELBOW;
			ChangeAnimation(Character_Eric::ANM_ElbowPunch);
			SetAttackStatus();
			bAction = true;
				break;
			// KICK
		case Character_Eric::ATK_MIDDLEKICK:
			if (bAction)break;
			m_nowAttack = Character_Eric::ATK_MIDDLEKICK;
			ChangeAnimation(Character_Eric::ANM_Kick);
			SetAttackStatus();
			bAction = true;
				break;
			// Knee
		case Character_Eric::ATK_KNEE:
			if (bAction)break;
			m_nowAttack = Character_Eric::ATK_KNEE;
			ChangeAnimation(Character_Eric::ANM_Knee);
			SetAttackStatus();
			bAction = true;
				break;

		default:
		{
			float dir = m_pOpponent->GetPos().x - this->GetPos().x;
			dir /= abs(dir);
			switch (m_State)
			{
				
			case Character_Eric::PS_WALK:
				m_localposition.x += FSPEED * dir;
				if (m_localposition.x <= LimitMinPos)
				{
					m_localposition.x = LimitMinPos;
				}
				else if (m_localposition.x >= LimitMaxPos)
				{
					m_localposition.x = LimitMaxPos;
				}
				break;
			case Character_Eric::PS_BACK:
				m_localposition.x -= BSPEED * dir;
				if (m_localposition.x <= LimitMinPos)
				{
					m_localposition.x = LimitMinPos;
				}
				else if (m_localposition.x >= LimitMaxPos)
				{
					m_localposition.x = LimitMaxPos;
				}
				break;
			default:break;
			}
		}
				break;
		}
	}m_nThinkCount++; 
	
}
//*****************************************************
// 攻撃するときの各種パラメータ設定をする関数
//*****************************************************
void CCharacterEric::SetAttackStatus()
{
	m_State = Character_Eric::PS_ATTACK;
	m_nAttackFrame = 0;
	m_bAlreadyActiveAttackHitBox = false;
	if (m_nowAttack < Character_Eric::ATK_NONE)
	{
		m_pNowAttackDataDetails = &m_AttackDataDetailsList[m_nowAttack];
		m_nRigidityFrame = m_pNowAttackDataDetails->pAttackData->OverallFrame;
	}
	else
	{
		m_pNowAttackDataDetails = nullptr;
	}
}
//*****************************************************
// 攻撃が終わったときの各種パラメータ設定をする関数
//*****************************************************
void CCharacterEric::SetEndAttackStatus()
{
	m_State = Character_Eric::PS_IDLE;
	m_nowAttack = Character_Eric::ATK_NONE;
	m_nAttackFrame = 0;
	m_bAlreadyActiveAttackHitBox = true;
	m_pNowAttackDataDetails = nullptr;
	ChangeAnimation(Character_Eric::ANM_Idle);
}

//*****************************************************
// 出した攻撃と相手のヒットボックスが当たったとき
//*****************************************************
void CCharacterEric::SetAttack()
{
	m_pAttackHitBox->SetUse(false);
}

//*****************************************************
// 相手の出した攻撃と自分のヒットボックスが当たったとき
//*****************************************************
void CCharacterEric::SetReceiveAttack(float Damage, int InflictHitRigidityFrame, int InflictGuardRigidityFrame, float HitBackDist, float GuardBackDist,eSE se)
{
	float work = 1.0f;
	float worksum = 1.0f;
	bool bThanLeft = (m_pOpponent->GetPos().x - this->GetPos().x) > 0;
	float ultBackDist = 0.0f;	// 最終的に下がる距離
	float surplusDist = 0.0f;	// 画面端に到達した場合の余剰距離


	if (m_State == Character_Eric::PS_GUARD || m_State == Character_Eric::PS_BACK)// ガードだったら
	{
		ultBackDist = GuardBackDist;
		m_State = Character_Eric::PS_GUARD;
		m_nReceiveRigidityFrame = m_nRigidityFrame = InflictGuardRigidityFrame;
		m_bUseGuardEffect = true;
		m_GuardEffect.SetPos(XMFLOAT3(GetBodyBox()->GetWorldMatrix()._41, GetBodyBox()->GetWorldMatrix()._42, GetBodyBox()->GetWorldMatrix()._43 - 50.0f));
		// 画面端に到達するかどうか確認する
		if (bThanLeft)
		{
			float workpos = m_localposition.x - GuardBackDist;
			// 到達していたら、相手に下がってもらう
			if (workpos < LimitMinPos)
			{
				surplusDist = abs(workpos - LimitMinPos);
				m_pOpponent->SetRetHitBack(surplusDist);
			}
		}
		else
		{
			float workpos = m_localposition.x + GuardBackDist;
			// 到達していたら、相手に下がってもらう
			if (workpos > LimitMaxPos)
			{
				surplusDist = abs(workpos - LimitMaxPos);
				m_pOpponent->SetRetHitBack(surplusDist);
			}
		}
		// 下がる距離を段々小さくするため、最初の下がる距離を計算
		for (int i = 0; i < InflictGuardRigidityFrame - 1; i++)
		{
			work *= HitBackDecPar;
			worksum += work;
		}
	}
	else
	{
		CSound::Play(se);
		m_fHp -= Damage;
		if (m_fHp <= 0)
		{
			ChangeAnimation(Character_Eric::ANM_DownForPunch);
			m_State = Character_Eric::PS_LOSE;
		}
		else
		{
			ultBackDist = HitBackDist;
			m_State = Character_Eric::PS_DAMAGE;
			ChangeAnimation(Character_Eric::ANM_HeadDamageSmall);
			m_nReceiveRigidityFrame = m_nRigidityFrame = InflictHitRigidityFrame;
			m_bUseHitEffect = true;
			m_HitEffect.SetSize(XMFLOAT2(100.0f, 100.0f));
			m_HitEffect.SetPos(XMFLOAT3(m_pOpponent->GetAttackBox()->GetWorldMatrix()._41, m_pOpponent->GetAttackBox()->GetWorldMatrix()._42, m_pOpponent->GetAttackBox()->GetWorldMatrix()._43 - 50.0f));
			// 画面端に到達するかどうか確認する
			if (bThanLeft)
			{
				float workpos = m_localposition.x - HitBackDist;
				// 到達していたら、相手に下がってもらう
				if (workpos < LimitMinPos)
				{
					surplusDist = abs(workpos - LimitMinPos);
					m_pOpponent->SetRetHitBack(surplusDist);
				}
			}
			else
			{
				float workpos = m_localposition.x + HitBackDist;
				// 到達していたら、相手に下がってもらう
				if (workpos > LimitMaxPos)
				{
					surplusDist = abs(workpos - LimitMaxPos);
					m_pOpponent->SetRetHitBack(surplusDist);
				}
			}
			// 下がる距離を段々小さくするため、最初の下がる距離を計算
			for (int i = 0; i < InflictHitRigidityFrame - 1; i++)
			{
				work *= HitBackDecPar;
				worksum += work;
			}
		}
	}
	m_fHitBackThisFrame = ((ultBackDist - surplusDist) / worksum) * work;
	if (bThanLeft) { m_fHitBackThisFrame *= -1; }
}
//******************************************************
// ヒットバックが跳ね返る(画面端)時に、それをセットする
//******************************************************
void CCharacterEric::SetRetHitBack(float RetHitBackDist)
{
	// 攻撃硬直終了までのフレームを計算
	int remainingFrame = m_pNowAttackDataDetails->pAttackData->OverallFrame - m_nAttackFrame;
	// ヒットバックの跳ね返りがあるとき、最初の下がる距離を計算
	float work = 1.0f;
	float worksum = 1.0f;
	bool bThanLeft = (m_pOpponent->GetPos().x - GetPos().x) > 0;
	for (int i = 0; i < remainingFrame - 1; i++)
	{
		work *= HitBackDecPar;
		worksum += work;
	}
	m_fHitBackThisFrame = (RetHitBackDist / worksum) * work;
	if (bThanLeft) { m_fHitBackThisFrame *= -1; }
}

//******************************************************
// 現在の出している攻撃データを取得する
//******************************************************
AttackData CCharacterEric::GetAttackData()
{
	if(m_pNowAttackDataDetails)
	return *m_pNowAttackDataDetails->pAttackData;
	AttackData ad = {0,0,0,0,0,0,0,0,0,0};
	return ad;

	
}

void CCharacterEric::CPUAIthinkUpdate()
{
	m_nThinkCount = 0;

	int randval1 = rand() % 100;
	int randval2 = rand() % 100;
	Character_Eric::PLAYER_STATE oldPS = m_State;
	
	if (m_localposition.x - m_pOpponent->GetPos().x > 300.0f)
	{// 距離が離れているとき
		if (randval1 < 70)
			m_State = Character_Eric::PS_WALK;
		else if (randval1 < 95)
			m_State = Character_Eric::PS_BACK;
		else
		{
			m_State = Character_Eric::PS_ATTACK;
			if (randval2 < 25)
				m_nowAttack = Character_Eric::ATK_PUNCH;
			else if (randval2 < 50)
				m_nowAttack = Character_Eric::ATK_ELBOW;
			else if (randval2 < 75)
				m_nowAttack = Character_Eric::ATK_MIDDLEKICK;
			else
				m_nowAttack = Character_Eric::ATK_KNEE;
		}
	}
	else if (m_localposition.x - m_pOpponent->GetPos().x < 100.0f)
	{// 距離がすごく近いとき
		if (randval1 < 10)
			m_State = Character_Eric::PS_WALK;
		else if (randval1 < 50)
			m_State = Character_Eric::PS_BACK;
		else
		{
			m_State = Character_Eric::PS_ATTACK;
			if (randval2 < 25)
				m_nowAttack = Character_Eric::ATK_PUNCH;
			else if (randval2 < 50)
				m_nowAttack = Character_Eric::ATK_ELBOW;
			else if (randval2 < 75)
				m_nowAttack = Character_Eric::ATK_MIDDLEKICK;
			else
				m_nowAttack = Character_Eric::ATK_KNEE;
		}
	}
	else
	{// 中間距離
		if (randval1 < 40)
			m_State = Character_Eric::PS_WALK;
		else if (randval1 < 60)
			m_State = Character_Eric::PS_BACK;
		else
		{
			m_State = Character_Eric::PS_ATTACK;
			if (randval2 < 25)
				m_nowAttack = Character_Eric::ATK_PUNCH;
			else if (randval2 < 50)
				m_nowAttack = Character_Eric::ATK_ELBOW;
			else if (randval2 < 75)
				m_nowAttack = Character_Eric::ATK_MIDDLEKICK;
			else
				m_nowAttack = Character_Eric::ATK_KNEE;
		}
	}

	// 行動が変わっていたらアニメーション更新
	if (m_State != oldPS)
	{
		switch (m_State)
		{
		case Character_Eric::PS_WALK:
		case Character_Eric::PS_BACK:
			ChangeAnimation(Character_Eric::ANM_Walk);
			break;
		case Character_Eric::PS_IDLE:
			ChangeAnimation(Character_Eric::ANM_Idle);
			break;
		}
	}

}

void CCharacterEric::HitStopUpdate()
{
	
}

void CCharacterEric::ModelCameraInit()
{// カメラ座標を反映
	m_pModel->SetCamera(CCamera::Get()->GetEye());
	// 光源上方を反映
	m_pModel->SetLight(GetCLight()->GetLight());

	CObject::Update();	// ワールドマトリックス計算(親のアップデート)
}
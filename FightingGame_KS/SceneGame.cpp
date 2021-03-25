#include "SceneGame.h"
#include "Object.h"
#include "HPbar.h"
#include "meshfield.h"

#define INITPOS_1P (XMFLOAT3(-200.0f,0.0f,0.0f))
#define INITROT_1P (XMFLOAT3(0.0f,90.0f,0.0f))
#define INITPOS_2P (XMFLOAT3(200.0f,0.0f,0.0f))
#define INITROT_2P (XMFLOAT3(0.0f,-90.0f,0.0f))


CSceneGame::CSceneGame()
{
	m_bDispDeb = false;
}


CSceneGame::~CSceneGame()
{

}

void CSceneGame::Collision()
{
	if (ColObbs(*m_pPlayCharacter[0]->GetBodyBox(), *m_pPlayCharacter[1]->GetBodyBox()))
	{	// 体同士が当たっている
		
		// キャラが押される処理
		float pos1 = m_pPlayCharacter[0]->GetWorldMatrix()._41;
		float pos2 = m_pPlayCharacter[1]->GetWorldMatrix()._41;
		float centerpos = (pos1 + pos2) / 2;
		if (fabsf(pos1 - pos2) < (m_pPlayCharacter[0]->GetBodyBox()->GetWorldSize().x ) + (m_pPlayCharacter[1]->GetBodyBox()->GetWorldSize().x))
		{	
			m_pPlayCharacter[0]->SetPos(XMFLOAT3(centerpos > pos1 ? centerpos - (m_pPlayCharacter[0]->GetBodyBox()->GetWorldSize().x) : centerpos + (m_pPlayCharacter[0]->GetBodyBox()->GetWorldSize().x), 0.0f, 0.0f));
			m_pPlayCharacter[1]->SetPos(XMFLOAT3(centerpos > pos2 ? centerpos - (m_pPlayCharacter[0]->GetBodyBox()->GetWorldSize().x) : centerpos + (m_pPlayCharacter[0]->GetBodyBox()->GetWorldSize().x), 0.0f, 0.0f));
		}

		// 画面端なら調整
		if (m_pPlayCharacter[0]->GetPos().x < LimitMinPos)
		{
			float distwk = LimitMinPos - m_pPlayCharacter[0]->GetPos().x;
			m_pPlayCharacter[0]->AddPos(XMFLOAT3(distwk, 0, 0));
			m_pPlayCharacter[1]->AddPos(XMFLOAT3(distwk, 0, 0));
		}
		else if (m_pPlayCharacter[1]->GetPos().x > LimitMaxPos)
		{
			float distwk = LimitMaxPos - m_pPlayCharacter[1]->GetPos().x;
			m_pPlayCharacter[0]->AddPos(XMFLOAT3(distwk, 0, 0));
			m_pPlayCharacter[1]->AddPos(XMFLOAT3(distwk, 0, 0));
		}
		else if (m_pPlayCharacter[1]->GetPos().x < LimitMinPos)
		{
			float distwk = LimitMinPos - m_pPlayCharacter[1]->GetPos().x;
			m_pPlayCharacter[0]->AddPos(XMFLOAT3(distwk, 0, 0));
			m_pPlayCharacter[1]->AddPos(XMFLOAT3(distwk, 0, 0));
		}
		else if (m_pPlayCharacter[0]->GetPos().x > LimitMaxPos)
		{
			float distwk = LimitMaxPos - m_pPlayCharacter[0]->GetPos().x;
			m_pPlayCharacter[0]->AddPos(XMFLOAT3(distwk, 0, 0));
			m_pPlayCharacter[1]->AddPos(XMFLOAT3(distwk, 0, 0));
		}
	}
	if (ColObbs(*m_pPlayCharacter[0]->GetAttackBox(), *m_pPlayCharacter[1]->GetAttackBox()))
	{	// 相打ち(出している攻撃同士があたった)
		
		// 攻撃データ取得
		AttackData ADp1, ADp2;
		ADp1 = m_pPlayCharacter[0]->GetAttackData();
		ADp2 = m_pPlayCharacter[1]->GetAttackData();
		// 0に攻撃をしたときの処理をする
		m_pPlayCharacter[0]->SetAttack();
		// 0に1の攻撃を与える
		m_pPlayCharacter[0]->SetReceiveAttack(ADp2.AtacckDamage, ADp2.InflictHitRigidityFrame, ADp2.InflictGuardRigidityFrame,ADp2.InflictHitBackDistance, ADp2.InflictGuardBackDistance,ADp2.useSE);
		// 1に攻撃をしたときの処理をする
		m_pPlayCharacter[1]->SetAttack();
		// 1に0の攻撃を与える
		m_pPlayCharacter[1]->SetReceiveAttack(ADp1.AtacckDamage, ADp1.InflictHitRigidityFrame, ADp1.InflictGuardRigidityFrame,ADp1.InflictHitBackDistance,ADp1.InflictGuardBackDistance, ADp1.useSE);
		// ヒットストップを設定
		if (ADp1.HitStopFrame >= ADp2.HitStopFrame)
			m_nHitStopFrame = ADp1.HitStopFrame;
		else
			m_nHitStopFrame = ADp2.HitStopFrame;

	}
	else if (ColObbs(*m_pPlayCharacter[0]->GetAttackBox(), *m_pPlayCharacter[1]->GetBodyBox()))
	{	// 0の攻撃が1の体にあったっている
		if (ColObbs(*m_pPlayCharacter[1]->GetAttackBox(), *m_pPlayCharacter[0]->GetBodyBox()))
		{	// 相打ち(どちらの攻撃も当たっている)

			// 攻撃データ取得
			AttackData ADp1, ADp2;
			ADp1 = m_pPlayCharacter[0]->GetAttackData();
			ADp2 = m_pPlayCharacter[1]->GetAttackData();
			// 0に攻撃をしたときの処理をする
			m_pPlayCharacter[0]->SetAttack();
			// 0に1の攻撃を与える
			m_pPlayCharacter[0]->SetReceiveAttack(ADp2.AtacckDamage, ADp2.InflictHitRigidityFrame, ADp2.InflictGuardRigidityFrame,ADp2.InflictHitBackDistance,ADp2.InflictGuardBackDistance, ADp2.useSE);
			// 1に攻撃をしたときの処理をする
			m_pPlayCharacter[1]->SetAttack();
			// 1に0の攻撃を与える
			m_pPlayCharacter[1]->SetReceiveAttack(ADp1.AtacckDamage, ADp1.InflictHitRigidityFrame, ADp1.InflictGuardRigidityFrame,ADp1.InflictHitBackDistance,ADp1.InflictGuardBackDistance, ADp1.useSE);
			// ヒットストップを設定
			if (ADp1.HitStopFrame >= ADp2.HitStopFrame)
				m_nHitStopFrame = ADp1.HitStopFrame;
			else
				m_nHitStopFrame = ADp2.HitStopFrame;
		}
		else
		{	// (1の攻撃が0の体にあったっていない且つ)0の攻撃が1の体にあたっている 
			// 攻撃データ取得
			AttackData AD = m_pPlayCharacter[0]->GetAttackData();
			// 0に攻撃をしたときの処理をする
			m_pPlayCharacter[0]->SetAttack();
			// 1に0の攻撃を与える
			m_pPlayCharacter[1]->SetReceiveAttack(AD.AtacckDamage, AD.InflictHitRigidityFrame, AD.InflictGuardRigidityFrame,AD.InflictHitBackDistance,AD.InflictGuardBackDistance, AD.useSE);
			// ヒットストップを設定
			m_nHitStopFrame = AD.HitStopFrame;
		}
	}
	else if (ColObbs(*m_pPlayCharacter[1]->GetAttackBox(), *m_pPlayCharacter[0]->GetBodyBox()))
	{	// (0の攻撃が1の体にあったっていない且つ)1の攻撃が0の体にあたっている
		// 攻撃データ取得
		AttackData AD = m_pPlayCharacter[1]->GetAttackData();
		// 1に攻撃をしたときの処理をする
		m_pPlayCharacter[1]->SetAttack();
		// 0に1の攻撃を与える
		m_pPlayCharacter[0]->SetReceiveAttack(AD.AtacckDamage, AD.InflictHitRigidityFrame, AD.InflictGuardRigidityFrame,AD.InflictHitBackDistance,AD.InflictGuardBackDistance, AD.useSE);
		// ヒットストップを設定
		m_nHitStopFrame = AD.HitStopFrame;
	}
}
HRESULT CSceneGame::Init()
{
	m_pBattleCamera = new CBattleCamera;
	m_pBattleCamera->Init();
	CCamera::Set(m_pBattleCamera);
	m_pPlayCharacter[0] = new CCharacterEric(false);
	m_pPlayCharacter[1] = new CCharacterEric(true);
	m_pPlayCharacter[0]->SetOpponent(m_pPlayCharacter[1]);
	m_pPlayCharacter[1]->SetOpponent(m_pPlayCharacter[0]);
	CObjectManager* pobjmng = CObjectManager::Get();
	pobjmng->AddObject(m_pPlayCharacter[0]);
	m_pPlayCharacter[0]->Init();
	m_pPlayCharacter[0]->SetPos(INITPOS_1P);
	m_pPlayCharacter[0]->SetRot(INITROT_1P);
	m_pPlayCharacter[0]->ModelCameraInit();
	pobjmng->AddObject(m_pPlayCharacter[1]);
	m_pPlayCharacter[1]->Init();
	m_pPlayCharacter[1]->SetPos(INITPOS_2P);
	m_pPlayCharacter[1]->SetRot(INITROT_2P);
	m_pPlayCharacter[1]->ModelCameraInit();

	m_pSky = new CSky;
	m_pSky->Init();
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 20, 20, 100.0f, 100.0f);
	InitHP();
	SetPlayerHP(m_pPlayCharacter[0]->GetHP());
	SetPlayerHPMAX(m_pPlayCharacter[0]->GetHP());
	SetEnemyHP(m_pPlayCharacter[1]->GetHP());
	SetEnemyHPMAX(m_pPlayCharacter[1]->GetHP());

	m_nHitStopFrame = 0;
	return S_OK;
}
void CSceneGame::Uninit()
{
	UninitHP();
	UninitMeshField();
	m_pSky->Uninit();
	SAFE_DELETE(m_pSky);
	for (int i = 0; i < 2; i++)
	{
		CObjectManager::Get()->DeleteObject(m_pPlayCharacter[i]);
		m_pPlayCharacter[i]->Uninit();
		SAFE_DELETE(m_pPlayCharacter[i]);
	}
	CCamera::Set(GetDefaultCamera());
	m_pBattleCamera->Uninit();
	SAFE_DELETE(m_pBattleCamera);
}
void CSceneGame::Update()
{
	m_pPlayCharacter[0]->GetInputPreservation()->Update();
	m_pPlayCharacter[1]->GetInputPreservation()->Update();
	if (m_nHitStopFrame > 0)
	{// ヒットストップ中はオブジェクトの更新なし
		for (int i = 0; i < 2; i++)
		{
			m_pPlayCharacter[i]->HitStopUpdate();
		}
		m_nHitStopFrame--;
	}
	else
	{
		CObjectManager::Get()->Update();
	}
	m_pBattleCamera->Update(m_pPlayCharacter[0]->GetPos(), m_pPlayCharacter[1]->GetPos());
	Collision();
	SetPlayerHP(m_pPlayCharacter[0]->GetHP());
	SetEnemyHP(m_pPlayCharacter[1]->GetHP());
	if (GetKeyTrigger(VK_B))
	{
		m_bDispDeb = !m_bDispDeb;
		for (int i = 0; i < 2; i++)
		{
			m_pPlayCharacter[i]->SetDrawDeb(m_bDispDeb);
		}
	}
	
	
}
void CSceneGame::Draw()
{
	// 前面カリング (FBXは表裏が反転するため)
	SetCullMode(CULLMODE_CW);
	// スカイドーム描画
	m_pSky->Draw();
	CObjectManager::Get()->Draw();
	// 背面カリング (通常は表面のみ描画)
	SetCullMode(CULLMODE_CCW);
	// 地面描画
	UpdateMeshField();
	DrawMeshField();
	// Zバッファ無効
	SetZBuffer(false);

	DrawHP();
	if(m_bDispDeb)
	m_pPlayCharacter[0]->DebugDraw();
}

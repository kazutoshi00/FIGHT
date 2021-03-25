// 自機クラス [Player.h]
#pragma once
#include "main.h"
#include "FbxModel.h"
#include "OBB.h"
#include "CBillboard.h"
#include "PlayCharacter.h"

enum SelectAxis
{
	SA_x,
	SA_y,
	SA_z,
};
enum SelectElement
{
	SE_Size,
	SE_Pos,
	SE_Rot,
};

namespace Character_Eric
{
	enum PLAYER_STATE
	{
		PS_IDLE,
		PS_ATTACK,
		PS_DAMAGE,
		PS_GUARD,
		PS_WALK,
		PS_BACK,
		PS_LOSE,
		PS_WIN,

		PS_MAX
	};

	enum ATTACK_KIND
	{
		ATK_PUNCH,
		ATK_ELBOW,
		ATK_HEADBUTT,
		ATK_MIDDLEKICK,
		ATK_HIGHKICK,
		ATK_KNEE,
		ATK_NONE,

		ATK_MAX
	};

	enum ANIMATION_KIND
	{
		ANM_Tpose = 0,
		ANM_Idle,
		ANM_Walk,
		ANM_Punch,
		ANM_ElbowPunch,
		ANM_Headbutt,
		ANM_Kick,
		ANM_HighKick,
		ANM_Knee,
		ANM_HeadDamageSmall,
		ANM_HeadDamageLarge,
		ANM_KickDamage,
		ANM_DownForPunch,
		ANM_DownForKick,
		ANM_IdleToPunch,
		ANM_IdleToKick,
		ANM_IdleToHeadbutt,
		ANM_IdleToHighKick,

		MAX_ANM
	};
}
FLOAT LenSegOnSeparateAxis(XMVECTOR *Sep, XMVECTOR *e1, XMVECTOR *e2, XMVECTOR *e3);
bool ColObbs(COBB &obb1, COBB &obb2);
class CCharacterEric:public CPlayCharacter
{
private:
	CFbxModel* m_pModel;	// FBX モデル

	bool m_bHit;			// 衝突有無
	bool m_bDispHitbox;

	CBillboard m_icon;
	CBillboard m_GuardEffect;
	CBillboard m_HitEffect;
	bool m_bUseGuardEffect;
	bool m_bUseHitEffect;

	SelectAxis m_SelectAxis;
	SelectElement m_SelectElement;

	int m_nAnmFrameCount;	// アニメーションフレーム

	void PlayerUpdate();	// プレイヤー操作時のアップデート
	void CPUUpdate();		// CPU操作時のアップデート
	void SetAttackStatus();	// 攻撃するときの各種パラメータ設定をする関数
	void SetEndAttackStatus();	// 攻撃が終わったときの各種パラメータ設定をする関数
	void CPUAIthinkUpdate();	// 敵AIの思考更新処理
public:
	Character_Eric::PLAYER_STATE m_State;
	Character_Eric::ATTACK_KIND m_nowAttack;
	Character_Eric::ANIMATION_KIND m_NowAnimation;
	void ChangeAnimation(Character_Eric::ANIMATION_KIND);
public:
	CCharacterEric(bool bCPU);		// コンストラクタ
	~CCharacterEric();		// デストラクタ

	HRESULT Init();	// 初期化
	void Uninit();	// 終了処理
	void Update();	// 更新
	void Draw();	// 描画
	void HitStopUpdate();
	void Reset();

	void SetHit(bool bHit);
	void SetAttack();	// 攻撃を当てたときに呼ぶ関数
	void SetReceiveAttack(float Damage, int InflictHitRigidityFrame, int InflictGuardRigidityFrame,float HitBackDist, float GuardBackDist, eSE se);	// 攻撃をくらった時に呼ぶ関数
	void SetRetHitBack(float RetHitBackDist);	// ヒットバックが跳ね返る(画面端)時に、それをセットする関数

	AttackData GetAttackData();		// 現在出している攻撃のデータを取得する

	void DebugDraw();
	void ModelCameraInit();
	void SetDrawDeb(bool flg);
};

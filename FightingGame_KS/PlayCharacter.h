#pragma once
#include "Object.h"
#include "OBB.h"
#include "InputPreservation.h"
#include <vector>
#include "Sound.h"

#define LimitMaxPos (900.0f)
#define LimitMinPos (-900.0f)
#define HitBackDecPar (1.2f)

struct AttackData
{
	int OverallFrame;		// 全体フレーム
	int OccurrenceFrame;	// 発生フレーム
	int SustainFrame;		// 持続フレーム
	int RigidityFrame;		// 硬直フレーム
	int InflictHitRigidityFrame;		// ヒット時の相手硬直フレーム
	int InflictGuardRigidityFrame;	// ガード時の相手硬直フレーム
	int HitStopFrame;		// 攻撃判定が相手にあたったとき(ガード時も)に両方のキャラのアニメーションが止まる時間
	float AtacckDamage;		// 攻撃力
	float InflictHitBackDistance;		// ヒットバック(ヒット時に相手が後ろに下がる距離)
	float InflictGuardBackDistance;		// ガードバック(ガード時に相手が後ろに下がる距離) 
	eSE useSE;
};

struct AnimationData
{
	int OverallFrame;		// アニメーションの全体フレーム
	int AnimationNumber;	// キャラクターに入っているアニメーションのうち何番目か
};

struct AttackAnimation
{
	AnimationData AnimationData;	// アニメーションの基本データ
	int OccurrenceFrame;			// 攻撃発生のアニメーションフレーム
	int SustainFrame;				// 攻撃持続のアニメーションフレーム
	int RigidityFrame;				// 攻撃硬直のアニメーションフレーム
};

class CPlayCharacter:public CObject 
{
public:
	struct AttackDataDetails
	{
		AttackData* pAttackData;	// 攻撃のデータ
		AttackAnimation* pAttackAnimaion;	// アニメーションのデータ
	};

protected:
	COBB* m_pBodyHitBox;
	COBB* m_pAttackHitBox;
	CPlayCharacter* m_pOpponent;	// 相手キャラ
	CInputPreservation* m_pInputPreservation;	// コマンド入力
	float m_fHp;
	float m_fMaxHp;
	int m_nRigidityFrame;		// 硬直時間
	int m_nReceiveRigidityFrame;	// 攻撃を受けたときの硬直時間(アニメーション用)
	float m_fHitBackThisFrame;	// 現在のフレームのヒットバック距離
	int m_nAttackFrame;			// 現在の攻撃を出してからの経過フレーム
	bool m_bAlreadyActiveAttackHitBox;	// 攻撃を開始してから、攻撃ヒットボックスが1度でもアクティブになったかどうか
	std::vector<AttackDataDetails> m_AttackDataDetailsList;	// 攻撃データのリスト
	AttackDataDetails* m_pNowAttackDataDetails;	// 今の攻撃のデータ
	bool m_bCPUPlay;	// CPUかどうか(true:cpu,false:player操作)
	int m_nThinkCount;	// CPU時の意思更新フレーム保存用
	virtual void PlayerUpdate() = 0;	// プレイヤー操作時のアップデート
	virtual void CPUUpdate() = 0;		// CPU操作時のアップデート
	virtual void SetAttackStatus() = 0;	// 攻撃するときの各種パラメータ設定をする関数
	virtual void SetEndAttackStatus() = 0;	// 攻撃が終わったときの各種パラメータ設定をする関数
	virtual void CPUAIthinkUpdate() = 0;	// 敵AIの思考更新処理
public:

	CPlayCharacter();
	~CPlayCharacter();
	virtual HRESULT Init() { return S_OK; }
	virtual void Uninit() {}
	virtual void Update() {}
	virtual void Draw() {}
	virtual void HitStopUpdate() = 0;


	COBB* GetBodyBox(){ return m_pBodyHitBox; }
	COBB* GetAttackBox(){ return m_pAttackHitBox; }
	void AddPos(XMFLOAT3 val) {
		m_localposition.x += val.x;
		m_localposition.y += val.y;
		m_localposition.z += val.z;
		CObject::UpdateMatrix();
	}
	void SetOpponent(CPlayCharacter* opponent) { m_pOpponent = opponent; }
	XMFLOAT3 GetPos();		// 座標取得
	XMFLOAT4X4& GetWorld();	// ワールド変換取得
	virtual void SetAttack() = 0;		// 攻撃を当てたときに呼ぶ関数
	virtual void SetReceiveAttack(float Damage,int InflictHitRigidityFrame,int InflictGuardRigidityFrame,float HitBackDist,float GuardBackDist,eSE se) = 0;	// 攻撃をくらった時に呼ぶ関数
	virtual void SetRetHitBack(float RetHitBackDist) = 0;	// ヒットバックが跳ね返る(画面端)時に、それをセットする関数
	virtual AttackData GetAttackData() = 0;		// 現在出している攻撃のデータを取得する
	float GetHP() { return m_fHp; }
	CInputPreservation* GetInputPreservation() { return m_pInputPreservation; };	// コマンド入力更新用ゲッター
	virtual void ModelCameraInit() = 0;
	virtual void DebugDraw() = 0;

	virtual void SetDrawDeb(bool flg) = 0;
};


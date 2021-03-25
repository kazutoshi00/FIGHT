// ���@�N���X [Player.h]
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
	CFbxModel* m_pModel;	// FBX ���f��

	bool m_bHit;			// �Փ˗L��
	bool m_bDispHitbox;

	CBillboard m_icon;
	CBillboard m_GuardEffect;
	CBillboard m_HitEffect;
	bool m_bUseGuardEffect;
	bool m_bUseHitEffect;

	SelectAxis m_SelectAxis;
	SelectElement m_SelectElement;

	int m_nAnmFrameCount;	// �A�j���[�V�����t���[��

	void PlayerUpdate();	// �v���C���[���쎞�̃A�b�v�f�[�g
	void CPUUpdate();		// CPU���쎞�̃A�b�v�f�[�g
	void SetAttackStatus();	// �U������Ƃ��̊e��p�����[�^�ݒ������֐�
	void SetEndAttackStatus();	// �U�����I������Ƃ��̊e��p�����[�^�ݒ������֐�
	void CPUAIthinkUpdate();	// �GAI�̎v�l�X�V����
public:
	Character_Eric::PLAYER_STATE m_State;
	Character_Eric::ATTACK_KIND m_nowAttack;
	Character_Eric::ANIMATION_KIND m_NowAnimation;
	void ChangeAnimation(Character_Eric::ANIMATION_KIND);
public:
	CCharacterEric(bool bCPU);		// �R���X�g���N�^
	~CCharacterEric();		// �f�X�g���N�^

	HRESULT Init();	// ������
	void Uninit();	// �I������
	void Update();	// �X�V
	void Draw();	// �`��
	void HitStopUpdate();
	void Reset();

	void SetHit(bool bHit);
	void SetAttack();	// �U���𓖂Ă��Ƃ��ɌĂԊ֐�
	void SetReceiveAttack(float Damage, int InflictHitRigidityFrame, int InflictGuardRigidityFrame,float HitBackDist, float GuardBackDist, eSE se);	// �U��������������ɌĂԊ֐�
	void SetRetHitBack(float RetHitBackDist);	// �q�b�g�o�b�N�����˕Ԃ�(��ʒ[)���ɁA������Z�b�g����֐�

	AttackData GetAttackData();		// ���ݏo���Ă���U���̃f�[�^���擾����

	void DebugDraw();
	void ModelCameraInit();
	void SetDrawDeb(bool flg);
};

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
	int OverallFrame;		// �S�̃t���[��
	int OccurrenceFrame;	// �����t���[��
	int SustainFrame;		// �����t���[��
	int RigidityFrame;		// �d���t���[��
	int InflictHitRigidityFrame;		// �q�b�g���̑���d���t���[��
	int InflictGuardRigidityFrame;	// �K�[�h���̑���d���t���[��
	int HitStopFrame;		// �U�����肪����ɂ��������Ƃ�(�K�[�h����)�ɗ����̃L�����̃A�j���[�V�������~�܂鎞��
	float AtacckDamage;		// �U����
	float InflictHitBackDistance;		// �q�b�g�o�b�N(�q�b�g���ɑ��肪���ɉ����鋗��)
	float InflictGuardBackDistance;		// �K�[�h�o�b�N(�K�[�h���ɑ��肪���ɉ����鋗��) 
	eSE useSE;
};

struct AnimationData
{
	int OverallFrame;		// �A�j���[�V�����̑S�̃t���[��
	int AnimationNumber;	// �L�����N�^�[�ɓ����Ă���A�j���[�V�����̂������Ԗڂ�
};

struct AttackAnimation
{
	AnimationData AnimationData;	// �A�j���[�V�����̊�{�f�[�^
	int OccurrenceFrame;			// �U�������̃A�j���[�V�����t���[��
	int SustainFrame;				// �U�������̃A�j���[�V�����t���[��
	int RigidityFrame;				// �U���d���̃A�j���[�V�����t���[��
};

class CPlayCharacter:public CObject 
{
public:
	struct AttackDataDetails
	{
		AttackData* pAttackData;	// �U���̃f�[�^
		AttackAnimation* pAttackAnimaion;	// �A�j���[�V�����̃f�[�^
	};

protected:
	COBB* m_pBodyHitBox;
	COBB* m_pAttackHitBox;
	CPlayCharacter* m_pOpponent;	// ����L����
	CInputPreservation* m_pInputPreservation;	// �R�}���h����
	float m_fHp;
	float m_fMaxHp;
	int m_nRigidityFrame;		// �d������
	int m_nReceiveRigidityFrame;	// �U�����󂯂��Ƃ��̍d������(�A�j���[�V�����p)
	float m_fHitBackThisFrame;	// ���݂̃t���[���̃q�b�g�o�b�N����
	int m_nAttackFrame;			// ���݂̍U�����o���Ă���̌o�߃t���[��
	bool m_bAlreadyActiveAttackHitBox;	// �U�����J�n���Ă���A�U���q�b�g�{�b�N�X��1�x�ł��A�N�e�B�u�ɂȂ������ǂ���
	std::vector<AttackDataDetails> m_AttackDataDetailsList;	// �U���f�[�^�̃��X�g
	AttackDataDetails* m_pNowAttackDataDetails;	// ���̍U���̃f�[�^
	bool m_bCPUPlay;	// CPU���ǂ���(true:cpu,false:player����)
	int m_nThinkCount;	// CPU���̈ӎv�X�V�t���[���ۑ��p
	virtual void PlayerUpdate() = 0;	// �v���C���[���쎞�̃A�b�v�f�[�g
	virtual void CPUUpdate() = 0;		// CPU���쎞�̃A�b�v�f�[�g
	virtual void SetAttackStatus() = 0;	// �U������Ƃ��̊e��p�����[�^�ݒ������֐�
	virtual void SetEndAttackStatus() = 0;	// �U�����I������Ƃ��̊e��p�����[�^�ݒ������֐�
	virtual void CPUAIthinkUpdate() = 0;	// �GAI�̎v�l�X�V����
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
	XMFLOAT3 GetPos();		// ���W�擾
	XMFLOAT4X4& GetWorld();	// ���[���h�ϊ��擾
	virtual void SetAttack() = 0;		// �U���𓖂Ă��Ƃ��ɌĂԊ֐�
	virtual void SetReceiveAttack(float Damage,int InflictHitRigidityFrame,int InflictGuardRigidityFrame,float HitBackDist,float GuardBackDist,eSE se) = 0;	// �U��������������ɌĂԊ֐�
	virtual void SetRetHitBack(float RetHitBackDist) = 0;	// �q�b�g�o�b�N�����˕Ԃ�(��ʒ[)���ɁA������Z�b�g����֐�
	virtual AttackData GetAttackData() = 0;		// ���ݏo���Ă���U���̃f�[�^���擾����
	float GetHP() { return m_fHp; }
	CInputPreservation* GetInputPreservation() { return m_pInputPreservation; };	// �R�}���h���͍X�V�p�Q�b�^�[
	virtual void ModelCameraInit() = 0;
	virtual void DebugDraw() = 0;

	virtual void SetDrawDeb(bool flg) = 0;
};


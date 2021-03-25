#pragma once
#include "main.h"
class CObject
{
private:
	XMFLOAT3 m_worldposition;	// �|�W�V����(�e���l������worldPos)
	XMFLOAT3 m_worldrotation;	// ��](�e���l������worldRot)
	XMFLOAT3 m_worldscale;		// �g�嗦(�e���l������worldScale)
	bool m_bUpdated;		// �X�V���ꂽ���ǂ���
	CObject* m_pChild;		// �q�I�u�W�F�N�g
	CObject* m_pBrotherNext;	// �Z��I�u�W�F�N�g(�q�I�u�W�F�N�g���X�g�̎��|�C���^)


protected:
	CObject* m_pParent;	// �e�I�u�W�F�N�g
	CObject* m_pNext;	// ���̃I�u�W�F�N�g
	CObject* m_pBack;	// �O�̃I�u�W�F�N�g

	XMFLOAT4X4 m_world;		// ���[���h �}�g���b�N�X
	XMFLOAT4X4 m_worldRotWork;	// ��]�����߂邽�߂ɕK�v�ȍs��̊i�[�ꏊ
	XMFLOAT3 m_localposition;	// �|�W�V����
	XMFLOAT3 m_localrotation;	// ��]
	XMFLOAT3 m_localscale;		// �g�嗦(�傫��)

	XMFLOAT3& GetWorldPosition() { return m_worldposition; }
	XMFLOAT3& GetWorldRotation() { return m_worldrotation; }
	XMFLOAT3& GetWorldScale() { return m_worldscale; }
	bool IsUpdated() { return m_bUpdated; }

public:
	CObject();
	~CObject();

	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	void UpdateMatrix();

	virtual void SetSize(XMFLOAT3 size);
	virtual void SetPos(XMFLOAT3 pos);
	virtual void SetRot(XMFLOAT3 rot);
	XMFLOAT4X4& GetWorldMatrix() { return m_world; };
	XMFLOAT4X4& GetWorldRotWorkMatrix() { return m_worldRotWork; };
	void SetNext(CObject* pNext) { m_pNext = pNext; }		// ���̃I�u�W�F�N�g�̃|�C���^���Z�b�g����(���X�g�\��)
	void SetBack(CObject*pBack) { m_pBack = pBack; }		// �O�̃I�u�W�F�N�g�̃|�C���^���Z�b�g����(���X�g�\��)
	CObject* GetNext() { return m_pNext; }			// ���̃I�u�W�F�N�g�̃|�C���^(���X�g�\��)
	CObject* GetBack() { return m_pBack; }			// ���̃I�u�W�F�N�g�̃|�C���^(���X�g�\��)
	
	void SetParent(CObject*);	// �e�I�u�W�F�N�g�̃Z�b�g
	void DeleteChild(CObject*);	// �����̃I�u�W�F�N�g���q���Ɏ��I�u�W�F�N�g����A���̏����폜����
	CObject* GetParent();	// �e�I�u�W�F�N�g�̎擾
};

class CObjectManager
{
private:
	static CObjectManager m_ObjectManager;
	CObject* m_pRootObj;
public:
	static CObjectManager* Get() { return &m_ObjectManager; }
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void AddObject(CObject*);
	void DeleteObject(CObject*);
};

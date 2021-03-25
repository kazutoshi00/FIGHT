#pragma once
#include "main.h"
#include "Object.h"
#include "mesh.h"
class COBB: public CObject
{
protected:
	XMVECTOR m_NormaDirect[3];   // �����x�N�g��
	float m_fLength[3];             // �e�������̒���

	MESH* m_pMesh;
	void SetMesh();

	bool m_bUse;
	bool m_bDispHitbox;		// �q�b�g�{�b�N�X��`�悷�邩�ǂ���

public:
	XMVECTOR GetDirect(int elem);   // �w�莲�ԍ��̕����x�N�g�����擾
	float GetLen_W(int elem);          // �w�莲�����̒������擾
	XMFLOAT3 GetPos_W();             // �ʒu���擾

public:
	COBB();
	~COBB();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void SetSize(XMFLOAT3);
	void SetPos(XMFLOAT3);
	void SetRot(XMFLOAT3);
	XMFLOAT3 GetSize();
	XMFLOAT3 GetPos();
	XMFLOAT3 GetRot();
	bool IsUse() { return m_bUse; }						// �q�b�g�{�b�N�X���A�N�e�B�u���ǂ���
	void SetUse(bool flg) { m_bUse = flg; }				// �q�b�g�{�b�N�X�̃A�N�e�B�u�̐؂�ւ�
	void SetDisp(bool disp) { m_bDispHitbox = disp; }	// �q�b�g�{�b�N�X�̕`��̐؂�ւ�

	XMFLOAT3 GetWorldSize() { return CObject::GetWorldScale(); }

};
bool ColObbs(COBB &obb1, COBB &obb2);



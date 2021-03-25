// �X�J�C�h�[��/�X�J�C�{�b�N�X [Sky.h]
#pragma once
#include "main.h"
#include "FbxModel.h"

class CSky
{
private:
	CFbxModel*	m_pModel;	// ���f��
	CFbxLight	m_lightOff;	// ��������

public:
	CSky();
	~CSky();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
};


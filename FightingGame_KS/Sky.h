// スカイドーム/スカイボックス [Sky.h]
#pragma once
#include "main.h"
#include "FbxModel.h"

class CSky
{
private:
	CFbxModel*	m_pModel;	// モデル
	CFbxLight	m_lightOff;	// 光源無効

public:
	CSky();
	~CSky();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
};


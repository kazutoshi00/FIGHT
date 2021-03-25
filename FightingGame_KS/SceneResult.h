#pragma once
#include "SceneBase.h"
class CSceneResult :
	public CSceneBase
{
public:
	CSceneResult();
	~CSceneResult();
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
private:
	ID3D11ShaderResourceView* m_pTexresultLogo;
	ID3D11ShaderResourceView* m_pTexresultBG;
};


#pragma once
#include "SceneBase.h"
class CSceneTitle :
	public CSceneBase
{
public:
	CSceneTitle();
	~CSceneTitle();
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
private :
	ID3D11ShaderResourceView* m_pTexTitleLogo;
	ID3D11ShaderResourceView* m_pTexTitleGuide;
	ID3D11ShaderResourceView* m_pTexTitleBG;
	float m_fTitleGuideAlpha;
	bool m_bTitleGuideAlphaFlg;
};


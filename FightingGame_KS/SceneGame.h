#pragma once
#include "SceneBase.h"
#include "PlayCharacter.h"
#include "CharacterEric.h"
#include "Sky.h"
#include "BattleCamera.h"

class CSceneGame :
	public CSceneBase
{
private :
	CPlayCharacter* m_pPlayCharacter[2];
	CSky* m_pSky;
	CBattleCamera* m_pBattleCamera;

	int m_nHitStopFrame;
	void Collision();
	bool m_bDispDeb;

public:
	CSceneGame();
	~CSceneGame();
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
};


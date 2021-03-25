#pragma once
#include "SceneBase.h"

enum SCENE_KIND
{
	TITLE,
	GAME,
	RESULT,
};

enum FADE_KIND
{
	FADE_NORMAL,
	FADE_KIND_MAX,
};

class CFade;
class CSceneManager
{
public:
	
	CSceneManager();
	~CSceneManager();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
	CSceneBase* m_pNowScene;
	void SetNextScene(SCENE_KIND scene);
private:
	bool m_bChangeSceneflg;
	SCENE_KIND m_storescene;
	SCENE_KIND m_nowscene;
	CFade *m_pfade;
public:
	friend void ChangeScene(CSceneManager*,enum SCENE_KIND);

};

class CFade
{
public:
	CFade();
	~CFade();

	void Init();
	void Uninit();
	void Update();
	void Draw();

	void StartFade(enum FADE_KIND fade);
	bool Isfade() { return m_bIsfade; }
	bool Isfadeout() { return m_bIsfadeout; }
	bool Isfadein() { return m_bIsfadein; }

private:
	ID3D11ShaderResourceView* m_pTexFade[FADE_KIND_MAX];
	bool m_bIsfade, m_bIsfadeout, m_bIsfadein;
	float m_fadeAlpha;
	FADE_KIND m_nowfade;
};

void ChangeScene(enum SCENE_KIND scene);
CSceneManager* GetSceneManager();
void CreateSceneManager();
void DeleteSceneManager();


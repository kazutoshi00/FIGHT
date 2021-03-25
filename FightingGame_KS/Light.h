#pragma once
#include "main.h"
#include "FbxModel.h"

enum LIGHT_KIND
{
	E_LIGHT_STAGE = 0,


	LIGHTNONE,
	LIGHTMAX
};

class CLight
{
private:
	CFbxLight m_pLight[LIGHTMAX - 1];
	CFbxLight* m_pNowLight;
public:
	CLight();
	~CLight();
	void Init();
	void Uninit();
	void Update();
	CFbxLight& GetLight();
	void SetLightFalse();
	void SetLightTrue();
	void SetLightTrue(enum LIGHT_KIND);

};

CLight* GetCLight();


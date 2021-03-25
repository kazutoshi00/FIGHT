#pragma once
#include "main.h"
#include "Camera.h"
class CBattleCamera :
	public CCamera
{
private:
	float m_distZ;
	XMFLOAT3 m_shakeVal;
	void Shake();
	int m_shakecounter;
	bool m_bIsSmallShake;
	bool m_bIsLargeShake;
public:
	CBattleCamera();
	~CBattleCamera();
	HRESULT Init();
	void Uninit();
	void Update(XMFLOAT3 CharcterPosition1, XMFLOAT3 CharcterPosition2 );

	void SetShake(bool);
};


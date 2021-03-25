#include "PlayCharacter.h"


CPlayCharacter::CPlayCharacter()
{
}


CPlayCharacter::~CPlayCharacter()
{
}

// À•Wæ“¾
XMFLOAT3 CPlayCharacter::GetPos()
{
	XMFLOAT3 vPos(m_world._41,
		m_world._42,
		m_world._43);
	return vPos;
}

// ƒ[ƒ‹ƒh•ÏŠ·æ“¾
XMFLOAT4X4& CPlayCharacter::GetWorld()
{
	return m_world;
}
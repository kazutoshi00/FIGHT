#include "PlayCharacter.h"


CPlayCharacter::CPlayCharacter()
{
}


CPlayCharacter::~CPlayCharacter()
{
}

// ���W�擾
XMFLOAT3 CPlayCharacter::GetPos()
{
	XMFLOAT3 vPos(m_world._41,
		m_world._42,
		m_world._43);
	return vPos;
}

// ���[���h�ϊ��擾
XMFLOAT4X4& CPlayCharacter::GetWorld()
{
	return m_world;
}
#include "Light.h"
#include "Camera.h"

#define LIGHT0_DIFFUSE	XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define LIGHT0_AMBIENT	XMFLOAT4(0.5f,0.5f,0.5f,1.0f)
#define LIGHT0_SPECULAR	XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define LIGHT0_DIR_X	(-1.0f)
#define LIGHT0_DIR_Y	(-1.0f)
#define LIGHT0_DIR_Z	(1.0f)


CLight g_CLight;



CLight::CLight()
{
	XMFLOAT3 vecDir;

	for (int i = 0; i < LIGHTMAX; i++)
	{
		switch (i)
		{ 
		case E_LIGHT_STAGE:
			m_pLight[i].m_diffuse = LIGHT0_DIFFUSE;
			m_pLight[i].m_ambient = LIGHT0_AMBIENT;
			m_pLight[i].m_specular = LIGHT0_SPECULAR;
			vecDir = XMFLOAT3(LIGHT0_DIR_X, LIGHT0_DIR_Y, LIGHT0_DIR_Z);
			XMStoreFloat3(&m_pLight[i].m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
			break;

		case LIGHTNONE:
			m_pLight[i].m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_pLight[i].m_ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			m_pLight[i].m_specular = LIGHT0_SPECULAR;
			m_pLight[i].m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
			break;
		}
	}

	Init();
}


CLight::~CLight()
{
}

void CLight::Init()
{
	SetLightTrue();
}
void CLight::Uninit()
{

}
void CLight::Update()
{
}


CFbxLight& CLight::GetLight()
{
	return *m_pNowLight;
}

void CLight::SetLightFalse()
{
	m_pNowLight = &m_pLight[LIGHTNONE];
}

void CLight::SetLightTrue()
{
	m_pNowLight = &m_pLight[E_LIGHT_STAGE];
}

void CLight::SetLightTrue(enum LIGHT_KIND kind)
{
	m_pNowLight = &m_pLight[kind];
}

CLight* GetCLight()
{
	return &g_CLight;
}
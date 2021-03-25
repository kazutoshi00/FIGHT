#ifndef __SHADER_RESOURCE_H__
#define __SHADER_RESOURCE_H__

#include "ShaderNEO.h"

//---------
// �����ɒ�`��ǉ�������Acpp�ɓǂݍ��ރt�@�C����ǉ�
enum VSKind
{
	VS_NORMAL,
	VS_BUMP,
	VS_PROJSHADOW,
	VS_DEPTHWRITE,
	VS_PMD,

	VS_MAX
};
enum PSKind
{
	PS_NORMAL,
	PS_COLOR,
	PS_LAMBERT,
	PS_PHONG,
	PS_BUMP,
	PS_PARALLAX,
	PS_PARAOCCLU,
	PS_PROJSHADOW,
	PS_DEPTHWRITE,
	PS_DEPTHSHADOW,
	PS_PMD,
	// �_�����A�X�|�b�g���C�g
	PS_LIGHTSOURCE,
	// �f�B�t�@�[�h�����_�����O(��������)
	PS_DEFERREDWRITE,
	// �f�B�t�@�[�h�����_�����O(��������)
	PS_DEFERREDLIGHT,

	PS_MAX
};

void InitShaderResource();
void UninitShaderResource();
void BindVS(VSKind vs);
void BindPS(PSKind ps);

#endif
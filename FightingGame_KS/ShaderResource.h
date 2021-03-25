#ifndef __SHADER_RESOURCE_H__
#define __SHADER_RESOURCE_H__

#include "ShaderNEO.h"

//---------
// ここに定義を追加したら、cppに読み込むファイルを追加
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
	// 点光源、スポットライト
	PS_LIGHTSOURCE,
	// ディファードレンダリング(書き込み)
	PS_DEFERREDWRITE,
	// ディファードレンダリング(光源処理)
	PS_DEFERREDLIGHT,

	PS_MAX
};

void InitShaderResource();
void UninitShaderResource();
void BindVS(VSKind vs);
void BindPS(PSKind ps);

#endif
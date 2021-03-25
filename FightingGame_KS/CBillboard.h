#pragma once
#include "main.h"
#include"mesh.h"

class CBillboard
{
protected:
	XMFLOAT3 m_pos;
	XMFLOAT3 m_scl;
	XMFLOAT3 m_rot;
	XMFLOAT2 m_size;
	XMFLOAT4 m_col;
	XMFLOAT4X4 m_mtxWorld;

	MESH m_mesh;
	MATERIAL m_material;



	HRESULT MakeVertex(ID3D11Device* pDevice);
	
public:

	virtual HRESULT Init(const wchar_t* Filename, XMFLOAT2 size);
	virtual HRESULT Init(XMFLOAT2 size);
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	void SetColor(XMFLOAT4 col);
	void SetPos(XMFLOAT3);
	void SetSize(XMFLOAT2 size);
	void SetScale(XMFLOAT3 scl);
	void SetRot(XMFLOAT3 rot);

};
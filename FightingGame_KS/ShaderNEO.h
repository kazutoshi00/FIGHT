#ifndef __SHADER_H__
#define __SHADER_H__

#include "main.h"


class ShaderBase
{
public:
	ShaderBase();
	virtual ~ShaderBase();

	HRESULT Create(const char* FileName);
	virtual void Bind() = 0;

protected:
	virtual HRESULT MakeShader(void* pData, UINT size) = 0;

};

class VertexShader : public ShaderBase
{
public:
	enum Layout
	{
		LAYOUT_PUN,		// pos-uv-normal
		LAYOUT_PUNT,	// pos-uv-normal-tangent
		LAYOUT_PCUN,	// pos-color-uv-normal
		LAYOUT_PMD,		// pmdƒ‚ƒfƒ‹
	};
public:
	VertexShader(Layout layout);
	virtual ~VertexShader();

	virtual void Bind();
protected:
	virtual HRESULT MakeShader(void* pData, UINT size);

private:
	Layout m_layout;
	ID3D11InputLayout* m_pInputLayout;
	ID3D11VertexShader* m_pVertexShader;
};

class PixelShader : public ShaderBase
{
public:
	PixelShader();
	virtual ~PixelShader();

	virtual void Bind();
protected:
	virtual HRESULT MakeShader(void* pData, UINT size);

private:
	ID3D11PixelShader* m_pPixelShader;
};

class ShaderBuffer
{
public:
	ShaderBuffer();
	~ShaderBuffer();

	HRESULT Create(UINT size);
	void Write(void* pData);
	void BindVS(UINT slot);
	void BindPS(UINT slot);
private:
	ID3D11Buffer* m_pBuffer;
};

void SetTextureVS(ID3D11ShaderResourceView* pTex, UINT slot = 0);
void SetTexturePS(ID3D11ShaderResourceView* pTex, UINT slot = 0);

#endif // __SHADER_H_
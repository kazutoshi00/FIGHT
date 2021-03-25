#include "ShaderNEO.h"
#include <stdio.h>

ShaderBase::ShaderBase()
{
}
ShaderBase::~ShaderBase()
{
}

HRESULT ShaderBase::Create(const char* FileName)
{
	HRESULT hr = E_FAIL;

	FILE *fp = NULL;
	fopen_s(&fp, FileName, "rb");
	if (fp == nullptr) { return hr; }

	// ファイルサイズ
	UINT size = 0;
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	// データ読み込み
	fseek(fp, 0L, SEEK_SET);
	BYTE *pData = new BYTE[size];
	fread_s(pData, size, size, 1, fp);
	fclose(fp);

	// シェーダ作成
	hr = MakeShader(pData, size);

	delete[] pData;
	return hr;
}


VertexShader::VertexShader(Layout layout)
	: m_pInputLayout(nullptr)
	, m_layout(layout)
{
}
VertexShader::~VertexShader()
{
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_pVertexShader);
}
void VertexShader::Bind()
{
	ID3D11DeviceContext* pContext = GetContext();
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->IASetInputLayout(m_pInputLayout);
}
HRESULT VertexShader::MakeShader(void* pData, UINT size)
{
	static const D3D11_INPUT_ELEMENT_DESC LayoutPUN[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static const D3D11_INPUT_ELEMENT_DESC LayoutPUNT[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static const D3D11_INPUT_ELEMENT_DESC LayoutPCUN[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	static const D3D11_INPUT_ELEMENT_DESC LayoutMMD[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,							 D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE",		0, DXGI_FORMAT_R16G16_UINT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHT",		0, DXGI_FORMAT_R8_UINT,				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "EDGE",		0, DXGI_FORMAT_R8_UINT,				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	static const D3D11_INPUT_ELEMENT_DESC* pLayout[] =
	{
		LayoutPUN, LayoutPUNT, LayoutPCUN, LayoutMMD
	};
	static const int layoutCnt[] =
	{
		_countof(LayoutPUN), _countof(LayoutPUNT), _countof(LayoutPCUN), _countof(LayoutMMD)
	};
	
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();

	// シェーダ生成
	hr = pDevice->CreateVertexShader(pData, size, nullptr, &m_pVertexShader);
	if (FAILED(hr)) { return hr; }

	// 頂点レイアウト作成
	hr = pDevice->CreateInputLayout(pLayout[m_layout], layoutCnt[m_layout], pData, size, &m_pInputLayout);
	if (FAILED(hr)) { return hr; }

	return hr;
}


PixelShader::PixelShader()
{
}
PixelShader::~PixelShader()
{
	SAFE_RELEASE(m_pPixelShader);
}
void PixelShader::Bind()
{
	ID3D11DeviceContext* pContext = GetContext();
	pContext->PSSetShader(m_pPixelShader, NULL, 0);
}
HRESULT PixelShader::MakeShader(void* pData, UINT size)
{
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();

	hr = pDevice->CreatePixelShader(pData, size, nullptr, &m_pPixelShader);
	if (FAILED(hr)) { return hr; }

	return hr;
}





ShaderBuffer::ShaderBuffer()
	:m_pBuffer(NULL)
{
}
ShaderBuffer::~ShaderBuffer()
{
	SAFE_RELEASE(m_pBuffer);
}

HRESULT ShaderBuffer::Create(UINT size)
{
	D3D11_BUFFER_DESC bufDesc = {};
	bufDesc.ByteWidth = size;
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//--- 頂点バッファの作成
	HRESULT hr;
	ID3D11Device* pDevice = GetDevice();
	hr = pDevice->CreateBuffer(&bufDesc, NULL, &m_pBuffer);
	return hr;
}
void ShaderBuffer::Write(void* pData)
{
	ID3D11DeviceContext* pContext = GetContext();
	pContext->UpdateSubresource(m_pBuffer, 0, NULL, pData, 0, 0);
}
void ShaderBuffer::BindVS(UINT slot)
{
	ID3D11DeviceContext* pContext = GetContext();
	pContext->VSSetConstantBuffers(slot, 1, &m_pBuffer);
}
void ShaderBuffer::BindPS(UINT slot)
{
	ID3D11DeviceContext* pContext = GetContext();
	pContext->PSSetConstantBuffers(slot, 1, &m_pBuffer);
}


void SetTextureVS(ID3D11ShaderResourceView* pTex, UINT slot)
{
	GetContext()->VSSetShaderResources(slot, 1, &pTex);
}
void SetTexturePS(ID3D11ShaderResourceView* pTex, UINT slot)
{
	GetContext()->PSSetShaderResources(slot, 1, &pTex);
}
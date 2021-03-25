// FBXファイル読込/表示 (FbxModel.h)
//#define D3DCOMPILER
#pragma once
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fbxsdk.h>
#ifdef D3DCOMPILER
#include <d3dcompiler.h>
#endif

#pragma comment(lib, "libfbxsdk-mt")

// マクロ
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{if(p){(p)->Release();(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{if(p){delete p;(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{if(p){delete[]p;(p)=nullptr;}}
#endif

#define MAX_REF_POLY			60	// 最大共有頂点参照数

// 定数
enum EByOpacity {
	eNoAffect = 0,		// 全て
	eOpacityOnly,		// 不透明のみ
	eTransparentOnly,	// 透明のみ
};

// マテリアル
struct TFbxMaterial {
	DirectX::XMFLOAT4	Ka;		// アンビエント+テクスチャ有無
	DirectX::XMFLOAT4	Kd;		// ディフューズ
	DirectX::XMFLOAT4	Ks;		// スペキュラ+スペキュラ強度
	DirectX::XMFLOAT4	Ke;		// エミッシブ
	ID3D11ShaderResourceView*	pTexture;		// 拡散テクスチャ
	ID3D11ShaderResourceView*	pTexEmmisive;	// 発光テクスチャ
	DWORD		dwNumFace;		// このマテリアルのポリゴン数
	TFbxMaterial()
	{
		Ka = DirectX::XMFLOAT4(0, 0, 0, 0);
		Kd = DirectX::XMFLOAT4(1, 1, 1, 1);
		Ks = DirectX::XMFLOAT4(0, 0, 0, 0);
		Ke = DirectX::XMFLOAT4(0, 0, 0, 0);
		pTexture = nullptr;
		pTexEmmisive = nullptr;
		dwNumFace = 0;
	}
	~TFbxMaterial()
	{
		SAFE_RELEASE(pTexture);
	}
};

// 頂点情報
struct TFbxVertex {
	DirectX::XMFLOAT3	vPos;	// 頂点位置
	DirectX::XMFLOAT3	vNorm;	// 頂点法線
	DirectX::XMFLOAT2	vTex;	// UV座標
	UINT		uBoneIndex[4];	// ボーン番号
	float		fBoneWeight[4];	// ボーン重み
	TFbxVertex()
	{
		vPos = DirectX::XMFLOAT3(0, 0, 0);
		vNorm = DirectX::XMFLOAT3(0, 0, 0);
		vTex = DirectX::XMFLOAT2(0, 0);
		uBoneIndex[0] = uBoneIndex[1] = uBoneIndex[2] = uBoneIndex[3] = 0;
		fBoneWeight[0] = fBoneWeight[1] = fBoneWeight[2] = fBoneWeight[3] = 0.0f;
	}
};

// ボーン
struct TBone {
	DirectX::XMFLOAT4X4 mBindPose;	// 初期ポーズ (ずっと変わらない)
	DirectX::XMFLOAT4X4 mNewPose;	// 現在のポーズ (その都度変わる)

	TBone()
	{
		XMStoreFloat4x4(&mBindPose, DirectX::XMMatrixIdentity());
		mNewPose = mBindPose;
	}
};

struct TSkinInfo {
	int				nNumBone;
	TBone*			pBoneArray;
	FbxCluster**	ppCluster;
	TSkinInfo()
	{
		nNumBone = 0;
		pBoneArray = nullptr;
		ppCluster = nullptr;
	}
};

// 1頂点の共有 (最大MAX_REF_POLYポリゴン)
struct TPolyTable {
	int nPolyIndex[MAX_REF_POLY];	// ポリゴン番号 
	int nIndex123[MAX_REF_POLY];	// 3つの頂点のうち、何番目か
	int nNumRef;					// 属しているポリゴン数

	TPolyTable()
	{
		ZeroMemory(nPolyIndex, sizeof(nPolyIndex));
		ZeroMemory(nIndex123, sizeof(nIndex123));
		nNumRef = 0;
	}
};

//
// FBXモデル光源
//
class CFbxLight
{
public:
	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT3 m_direction;

	CFbxLight() : m_diffuse(1.f, 1.f, 1.f, 1.f),
				  m_ambient(0.f, 0.f, 0.f, 1.f),
				  m_specular(0.f, 0.f, 0.f, 1.f),
				  m_direction(0.f, 0.f, 1.f)
	{
	}
};

// メッシュ クラス
class CFbxMesh
{
public:
	CFbxMesh();
	~CFbxMesh();

	CFbxMesh** m_ppChild;	// 自分の子へのポインタ
	DWORD m_dwNumChild;		// 子の数

public:
	// 外部のデバイス等情報
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	FbxNode* m_pFBXNode;						// FBXから姿勢行列を取り出す際に使うFBXポインタ
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	CFbxLight* m_pLight;
	DirectX::XMFLOAT3* m_pCamera;
	// アニメーション関連
	DirectX::XMFLOAT4X4 m_mParentOrientation;	// 親の姿勢行列
	DirectX::XMFLOAT4X4 m_mFBXOrientation;		// 自分の姿勢行列 (親から見た相対姿勢)
	DirectX::XMFLOAT4X4 m_mFinalWorld;			// 最終的なワールド行列 (この姿勢でレンダリングする)
	TFbxMaterial* m_pMateUsr;
	void SetTexture(ID3D11ShaderResourceView* pTex);

private:
	// メッシュ関連
	DWORD m_dwNumVert;
	DWORD m_dwNumFace;
	DWORD m_dwNumUV;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;
	TFbxMaterial* m_pMaterial;
	DWORD m_dwNumMaterial;
	// ↓頂点配列/インデックス配列を追加.
	TFbxVertex* m_pVertex;	// 頂点配列
	int** m_ppIndex;		// インデックス配列(の配列)
	// ボーン
	int m_nNumSkin;
	TSkinInfo* m_pBoneTable;
	ID3D11Buffer* m_pConstantBufferBone;

public:
	// メソッド
	HRESULT CreateFromFBX(FbxMesh* pFbxMesh);
	void RenderMesh(EByOpacity byOpacity = eNoAffect);
	void SetNewPoseMatrices(int nFrame);
	// ↓追加2
	int GetVertexCount();
	int GetVertex(TFbxVertex* pVertex, int nCount);
	int GetIndexCount();
	int GetIndex(int* pIndex, int nCount, int& nTop);

	void CalcBoundingBox(DirectX::XMFLOAT3& vMin,
		DirectX::XMFLOAT3& vMax);
	void CalcBoundingSphere(DirectX::XMFLOAT3& vCenter,
		float& fRadius);

	// 自作関数
	void SetNewPoseMatrices(int nFrame,int nOldFrame,float blendPar);
private:
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	HRESULT ReadSkinInfo(FbxMesh* pFbxMesh, TFbxVertex* pvVB, TPolyTable* PolyTable);
public:	// もともとprivateだった
	DirectX::XMFLOAT4X4 GetCurrentPoseMatrix(int nSkin, int nIndex);
};

//
// FBXモデル クラス
//
class CFbxModel
{
public:
	CFbxModel();
	~CFbxModel();

	// メソッド
	void Render(DirectX::XMFLOAT4X4& mWorld, DirectX::XMFLOAT4X4& mView, DirectX::XMFLOAT4X4& mProj, EByOpacity byOpacity = eNoAffect);
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LPCSTR pszFileName);
	void SetLight(CFbxLight& light);
	void SetCamera(DirectX::XMFLOAT3& vCamera);
	void SetAnimFrame(int nFrame);
	int GetMaxAnimFrame();
	int GetMinAnimFrame();
	void SetAnimStack(int nAnimStack);
	int GetMaxAnimStack();
	void SetMaterial(TFbxMaterial* pMaterial = nullptr);
	// ↓追加2
	int GetVertexCount();
	int GetVertex(TFbxVertex* pVertex, int nCount);
	int GetIndexCount();
	int GetIndex(int* pIndex, int nCount);

	DirectX::XMFLOAT3& GetCenter();	// 境界ボックス/球 中心座標
	DirectX::XMFLOAT3& GetBBox();	// 境界ボックス サイズ
	float GetRadius();				// 境界球半径
	
	// ---自作関数----------------------------------
	// モデルのテクスチャ設定
	CFbxMesh* GetRootMesh();
	void SetTexture(ID3D11ShaderResourceView* pTex);
	// アニメーションブレンド
	void SetAnimStackBlend(int nAnimStack,int BlendFrame);
	void SetAnimFrame(int nFrame,int nOldFrame);
	void SetBlendFrameAdd(int val);
	
	int GetMaxAnimFrameOld();
	int GetMinAnimFrameOld();

private:
	CFbxMesh* m_pRootMesh;
	// 外部のデバイス等情報
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext *m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	DirectX::XMFLOAT4X4 m_mWorld;
	// FBX
	FbxManager* m_pSdkManager;
	FbxImporter* m_pImporter;
	FbxScene* m_pScene;
	DirectX::XMFLOAT4X4 m_mFinalWorld;//最終的なワールド行列（この姿勢でレンダリングする）
	CFbxLight m_light;
	DirectX::XMFLOAT3 m_vCamera;
	TFbxMaterial* m_pMaterial;
	TFbxMaterial m_material;

	DirectX::XMFLOAT3 m_vCenter;
	DirectX::XMFLOAT3 m_vBBox;
	float m_fRadius;

private:
	// アニメーションフレーム
	int m_nAnimFrame;
	FbxTime m_tStart;
	FbxTime m_tStop;
	int m_nAnimStack;
	FbxArray<FbxString*> m_strAnimStackName;
	// アニメーションフレーム(ブレンド用一つ前)
	int m_nBlendFrameMax;
	int m_nBlendFrameNow;
	int m_nAnimFrameOld;
	int m_nAnimStackOld;
	FbxTime m_tStartOld;
	FbxTime m_tStopOld;

	HRESULT InitShader();
	HRESULT InitFBX(LPCSTR szFileName);
	HRESULT CreateFromFBX(LPCSTR szFileName);
	HRESULT LoadRecursive(FbxNode* pNode, CFbxMesh* pFBXMesh);
	void RenderMesh(CFbxMesh* pMesh, EByOpacity byOpacity);
	void RecursiveRender(CFbxMesh* pMesh, EByOpacity byOpacity);
	void SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame);
	void DestroyMesh(CFbxMesh* pMesh);

	void CalcBoundingSphere();

	// 自作関数
	void SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame,int nOldFrame);
	
};

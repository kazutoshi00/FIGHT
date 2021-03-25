// FBX�t�@�C���Ǎ�/�\�� (FbxModel.h)
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

// �}�N��
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{if(p){(p)->Release();(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{if(p){delete p;(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{if(p){delete[]p;(p)=nullptr;}}
#endif

#define MAX_REF_POLY			60	// �ő勤�L���_�Q�Ɛ�

// �萔
enum EByOpacity {
	eNoAffect = 0,		// �S��
	eOpacityOnly,		// �s�����̂�
	eTransparentOnly,	// �����̂�
};

// �}�e���A��
struct TFbxMaterial {
	DirectX::XMFLOAT4	Ka;		// �A���r�G���g+�e�N�X�`���L��
	DirectX::XMFLOAT4	Kd;		// �f�B�t���[�Y
	DirectX::XMFLOAT4	Ks;		// �X�y�L����+�X�y�L�������x
	DirectX::XMFLOAT4	Ke;		// �G�~�b�V�u
	ID3D11ShaderResourceView*	pTexture;		// �g�U�e�N�X�`��
	ID3D11ShaderResourceView*	pTexEmmisive;	// �����e�N�X�`��
	DWORD		dwNumFace;		// ���̃}�e���A���̃|���S����
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

// ���_���
struct TFbxVertex {
	DirectX::XMFLOAT3	vPos;	// ���_�ʒu
	DirectX::XMFLOAT3	vNorm;	// ���_�@��
	DirectX::XMFLOAT2	vTex;	// UV���W
	UINT		uBoneIndex[4];	// �{�[���ԍ�
	float		fBoneWeight[4];	// �{�[���d��
	TFbxVertex()
	{
		vPos = DirectX::XMFLOAT3(0, 0, 0);
		vNorm = DirectX::XMFLOAT3(0, 0, 0);
		vTex = DirectX::XMFLOAT2(0, 0);
		uBoneIndex[0] = uBoneIndex[1] = uBoneIndex[2] = uBoneIndex[3] = 0;
		fBoneWeight[0] = fBoneWeight[1] = fBoneWeight[2] = fBoneWeight[3] = 0.0f;
	}
};

// �{�[��
struct TBone {
	DirectX::XMFLOAT4X4 mBindPose;	// �����|�[�Y (�����ƕς��Ȃ�)
	DirectX::XMFLOAT4X4 mNewPose;	// ���݂̃|�[�Y (���̓s�x�ς��)

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

// 1���_�̋��L (�ő�MAX_REF_POLY�|���S��)
struct TPolyTable {
	int nPolyIndex[MAX_REF_POLY];	// �|���S���ԍ� 
	int nIndex123[MAX_REF_POLY];	// 3�̒��_�̂����A���Ԗڂ�
	int nNumRef;					// �����Ă���|���S����

	TPolyTable()
	{
		ZeroMemory(nPolyIndex, sizeof(nPolyIndex));
		ZeroMemory(nIndex123, sizeof(nIndex123));
		nNumRef = 0;
	}
};

//
// FBX���f������
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

// ���b�V�� �N���X
class CFbxMesh
{
public:
	CFbxMesh();
	~CFbxMesh();

	CFbxMesh** m_ppChild;	// �����̎q�ւ̃|�C���^
	DWORD m_dwNumChild;		// �q�̐�

public:
	// �O���̃f�o�C�X�����
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	FbxNode* m_pFBXNode;						// FBX����p���s������o���ۂɎg��FBX�|�C���^
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	CFbxLight* m_pLight;
	DirectX::XMFLOAT3* m_pCamera;
	// �A�j���[�V�����֘A
	DirectX::XMFLOAT4X4 m_mParentOrientation;	// �e�̎p���s��
	DirectX::XMFLOAT4X4 m_mFBXOrientation;		// �����̎p���s�� (�e���猩�����Ύp��)
	DirectX::XMFLOAT4X4 m_mFinalWorld;			// �ŏI�I�ȃ��[���h�s�� (���̎p���Ń����_�����O����)
	TFbxMaterial* m_pMateUsr;
	void SetTexture(ID3D11ShaderResourceView* pTex);

private:
	// ���b�V���֘A
	DWORD m_dwNumVert;
	DWORD m_dwNumFace;
	DWORD m_dwNumUV;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;
	TFbxMaterial* m_pMaterial;
	DWORD m_dwNumMaterial;
	// �����_�z��/�C���f�b�N�X�z���ǉ�.
	TFbxVertex* m_pVertex;	// ���_�z��
	int** m_ppIndex;		// �C���f�b�N�X�z��(�̔z��)
	// �{�[��
	int m_nNumSkin;
	TSkinInfo* m_pBoneTable;
	ID3D11Buffer* m_pConstantBufferBone;

public:
	// ���\�b�h
	HRESULT CreateFromFBX(FbxMesh* pFbxMesh);
	void RenderMesh(EByOpacity byOpacity = eNoAffect);
	void SetNewPoseMatrices(int nFrame);
	// ���ǉ�2
	int GetVertexCount();
	int GetVertex(TFbxVertex* pVertex, int nCount);
	int GetIndexCount();
	int GetIndex(int* pIndex, int nCount, int& nTop);

	void CalcBoundingBox(DirectX::XMFLOAT3& vMin,
		DirectX::XMFLOAT3& vMax);
	void CalcBoundingSphere(DirectX::XMFLOAT3& vCenter,
		float& fRadius);

	// ����֐�
	void SetNewPoseMatrices(int nFrame,int nOldFrame,float blendPar);
private:
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	HRESULT ReadSkinInfo(FbxMesh* pFbxMesh, TFbxVertex* pvVB, TPolyTable* PolyTable);
public:	// ���Ƃ���private������
	DirectX::XMFLOAT4X4 GetCurrentPoseMatrix(int nSkin, int nIndex);
};

//
// FBX���f�� �N���X
//
class CFbxModel
{
public:
	CFbxModel();
	~CFbxModel();

	// ���\�b�h
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
	// ���ǉ�2
	int GetVertexCount();
	int GetVertex(TFbxVertex* pVertex, int nCount);
	int GetIndexCount();
	int GetIndex(int* pIndex, int nCount);

	DirectX::XMFLOAT3& GetCenter();	// ���E�{�b�N�X/�� ���S���W
	DirectX::XMFLOAT3& GetBBox();	// ���E�{�b�N�X �T�C�Y
	float GetRadius();				// ���E�����a
	
	// ---����֐�----------------------------------
	// ���f���̃e�N�X�`���ݒ�
	CFbxMesh* GetRootMesh();
	void SetTexture(ID3D11ShaderResourceView* pTex);
	// �A�j���[�V�����u�����h
	void SetAnimStackBlend(int nAnimStack,int BlendFrame);
	void SetAnimFrame(int nFrame,int nOldFrame);
	void SetBlendFrameAdd(int val);
	
	int GetMaxAnimFrameOld();
	int GetMinAnimFrameOld();

private:
	CFbxMesh* m_pRootMesh;
	// �O���̃f�o�C�X�����
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
	DirectX::XMFLOAT4X4 m_mFinalWorld;//�ŏI�I�ȃ��[���h�s��i���̎p���Ń����_�����O����j
	CFbxLight m_light;
	DirectX::XMFLOAT3 m_vCamera;
	TFbxMaterial* m_pMaterial;
	TFbxMaterial m_material;

	DirectX::XMFLOAT3 m_vCenter;
	DirectX::XMFLOAT3 m_vBBox;
	float m_fRadius;

private:
	// �A�j���[�V�����t���[��
	int m_nAnimFrame;
	FbxTime m_tStart;
	FbxTime m_tStop;
	int m_nAnimStack;
	FbxArray<FbxString*> m_strAnimStackName;
	// �A�j���[�V�����t���[��(�u�����h�p��O)
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

	// ����֐�
	void SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame,int nOldFrame);
	
};

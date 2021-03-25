// FBX�t�@�C���Ǎ�/�\�� (FbxModel.cpp)
#include "FbxModel.h"
#include "Texture.h"
#include "Shader.h"

#pragma comment(lib, "d3d11")
#ifdef D3DCOMPILER
#pragma comment(lib, "d3dCompiler")
#endif
#pragma comment(lib, "libfbxsdk-mt")
#pragma comment(lib, "libxml2-mt")
#pragma comment(lib, "zlib-mt")

#define MAX_BONE_MATRIX	256

using namespace DirectX;

// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mW;			// ���[���h�s��(�]�u�s��)
	XMVECTOR	vEye;		// ���_���W
	XMVECTOR	vLightDir;	// ��������
	XMVECTOR	vLa;		// �����F(�A���r�G���g)
	XMVECTOR	vLd;		// �����F(�f�B�t���[�Y)
	XMVECTOR	vLs;		// �����F(�X�y�L����)
};

// �}�e���A�� (�V�F�[�_�p)
struct SHADER_MATERIAL {
	XMVECTOR	vAmbient;	// �A���r�G���g�F
	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
	XMVECTOR	vSpecular;	// �X�y�L�����F
	XMVECTOR	vEmissive;	// �G�~�b�V�u�F
};

// �V�F�[�_�ɓn���{�[���s��z��
struct SHADER_BONE {
	XMMATRIX mBone[MAX_BONE_MATRIX];
	SHADER_BONE()
	{
		for (int i = 0; i < MAX_BONE_MATRIX; i++) {
			mBone[i] = XMMatrixIdentity();
		}
	}
};

//---------------------------------------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------------------------------------
CFbxMesh::CFbxMesh()
{
	m_ppChild = nullptr;
	m_dwNumChild = 0;
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_pSampleLinear = nullptr;
	m_pConstantBuffer0 = nullptr;
	m_pConstantBuffer1 = nullptr;
	m_pConstantBufferBone = nullptr;
	m_pFBXNode = nullptr;
	XMStoreFloat4x4(&m_mView, XMMatrixIdentity());
	m_mProj = m_mView;
	m_mParentOrientation = m_mView;
	m_mFBXOrientation = m_mView;
	m_mFinalWorld = m_mView;
	m_pMateUsr = nullptr;
	m_pLight = nullptr;
	m_pCamera = nullptr;
	m_dwNumVert = 0;
	m_dwNumFace = 0;
	m_dwNumUV = 0;
	m_pVertexBuffer = nullptr;
	m_ppIndexBuffer = nullptr;
	m_pMaterial = nullptr;
	m_dwNumMaterial = 0;
	m_nNumSkin = 0;
	m_pBoneTable = nullptr;
	// ���ǉ�
	m_pVertex = nullptr;
	m_ppIndex = nullptr;
}

//---------------------------------------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------------------------------------
CFbxMesh::~CFbxMesh()
{
	// ���ǉ�
	SAFE_DELETE_ARRAY(m_pVertex);
	if (m_ppIndex) {
		for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
			SAFE_DELETE_ARRAY(m_ppIndex[i]);
		}
		SAFE_DELETE_ARRAY(m_ppIndex);
	}

	SAFE_DELETE_ARRAY(m_pMaterial);
	SAFE_RELEASE(m_pVertexBuffer);
	for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
		SAFE_RELEASE(m_ppIndexBuffer[i]);
	}
	SAFE_DELETE_ARRAY(m_ppIndexBuffer);
	SAFE_DELETE_ARRAY(m_ppChild);
	SAFE_RELEASE(m_pConstantBufferBone);
}

//---------------------------------------------------------------------------------------
// ���b�V������
//---------------------------------------------------------------------------------------
HRESULT CFbxMesh::CreateFromFBX(FbxMesh* pFbxMesh)
{
	HRESULT hr = S_OK;
	// ���O�Ƀ|���S�����A���_���A�e�N�X�`�����W���𒲂ׂ�
	m_dwNumFace = pFbxMesh->GetPolygonCount();
	m_dwNumVert = pFbxMesh->GetControlPointsCount();
	m_dwNumUV = pFbxMesh->GetTextureUVCount();
	if (m_dwNumFace == 0 || m_dwNumVert == 0)
		return hr;
	// �ꎞ�I�ȃ������m�� (���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@)
	TFbxVertex* pvVB = new TFbxVertex[(m_dwNumVert < m_dwNumUV) ? m_dwNumUV : m_dwNumVert];
	// �|���S�����Ƃɒ��_�ǂݍ��݁A�@���ǂݍ��݁AUV�ǂݍ���
	FbxVector4 Normal, Coord;
	int iStartIndex, iIndex0, iIndex1, iIndex2;
	int* piIndex;
	int index, UVindex;
	FbxVector2 v2;
	FbxGeometryElementUV* pUV = nullptr;
	FbxLayerElement::EMappingMode eMapMode = FbxLayerElement::eNone;
	FbxLayerElement::EReferenceMode eRefMode = FbxLayerElement::eDirect;
	const FbxLayerElementArrayTemplate<int>* pUVIndex = nullptr;
	const FbxLayerElementArrayTemplate<FbxVector2>* pUVDirect = nullptr;
	if (m_dwNumUV) {
		pUV = pFbxMesh->GetElementUV();
		eMapMode = pUV->GetMappingMode();
		eRefMode = pUV->GetReferenceMode();
		pUVIndex = &pUV->GetIndexArray();
		pUVDirect = &pUV->GetDirectArray();
	}
	bool bPolygonVertex = (m_dwNumUV && eMapMode == FbxLayerElement::eByPolygonVertex);
	for (DWORD i = 0; i < m_dwNumFace; ++i) {
		if (m_dwNumVert < m_dwNumUV) {	// UV���̂ق��������ꍇ�͖{����(���_�x�[�X)�C���f�b�N�X�𗘗p���Ȃ��BUV�C���f�b�N�X����ɂ���
			iIndex0 = pFbxMesh->GetTextureUVIndex(i, 0);
			iIndex1 = pFbxMesh->GetTextureUVIndex(i, 1);
			iIndex2 = pFbxMesh->GetTextureUVIndex(i, 2);
		} else {
			iStartIndex = pFbxMesh->GetPolygonVertexIndex(i);
			piIndex = pFbxMesh->GetPolygonVertices();	//�i���_�C���f�b�N�X�j�ǂݍ���
			iIndex0 = piIndex[iStartIndex + 0];
			iIndex1 = piIndex[iStartIndex + 1];
			iIndex2 = piIndex[iStartIndex + 2];
		}
		// ���_
		index = pFbxMesh->GetPolygonVertex(i, 0);
		Coord = pFbxMesh->GetControlPointAt(index);
		pvVB[iIndex0].vPos.x = static_cast<float>(-Coord[0]);
		pvVB[iIndex0].vPos.y = static_cast<float>(Coord[1]);
		pvVB[iIndex0].vPos.z = static_cast<float>(Coord[2]);

		index = pFbxMesh->GetPolygonVertex(i, 1);
		Coord = pFbxMesh->GetControlPointAt(index);
		pvVB[iIndex1].vPos.x = static_cast<float>(-Coord[0]);
		pvVB[iIndex1].vPos.y = static_cast<float>(Coord[1]);
		pvVB[iIndex1].vPos.z = static_cast<float>(Coord[2]);

		index = pFbxMesh->GetPolygonVertex(i, 2);
		Coord = pFbxMesh->GetControlPointAt(index);
		pvVB[iIndex2].vPos.x = static_cast<float>(-Coord[0]);
		pvVB[iIndex2].vPos.y = static_cast<float>(Coord[1]);
		pvVB[iIndex2].vPos.z = static_cast<float>(Coord[2]);
		// �@��
		pFbxMesh->GetPolygonVertexNormal(i, 0, Normal);
		pvVB[iIndex0].vNorm.x = static_cast<float>(-Normal[0]);
		pvVB[iIndex0].vNorm.y = static_cast<float>(Normal[1]);
		pvVB[iIndex0].vNorm.z = static_cast<float>(Normal[2]);

		pFbxMesh->GetPolygonVertexNormal(i, 1, Normal);
		pvVB[iIndex1].vNorm.x = static_cast<float>(-Normal[0]);
		pvVB[iIndex1].vNorm.y = static_cast<float>(Normal[1]);
		pvVB[iIndex1].vNorm.z = static_cast<float>(Normal[2]);

		pFbxMesh->GetPolygonVertexNormal(i, 2, Normal);
		pvVB[iIndex2].vNorm.x = static_cast<float>(-Normal[0]);
		pvVB[iIndex2].vNorm.y = static_cast<float>(Normal[1]);
		pvVB[iIndex2].vNorm.z = static_cast<float>(Normal[2]);
		// �e�N�X�`�����W
		if (bPolygonVertex) {
			// UV�}�b�s���O���[�h��ByPolygonVertex�̎�
			UVindex = pFbxMesh->GetTextureUVIndex(i, 0);
			v2 = pUVDirect->GetAt(UVindex);
			pvVB[iIndex0].vTex.x = static_cast<float>(v2[0]);
			pvVB[iIndex0].vTex.y = static_cast<float>(1.0 - v2[1]);

			UVindex = pFbxMesh->GetTextureUVIndex(i, 1);
			v2 = pUVDirect->GetAt(UVindex);
			pvVB[iIndex1].vTex.x = static_cast<float>(v2[0]);
			pvVB[iIndex1].vTex.y = static_cast<float>(1.0 - v2[1]);

			UVindex = pFbxMesh->GetTextureUVIndex(i, 2);
			v2 = pUVDirect->GetAt(UVindex);
			pvVB[iIndex2].vTex.x = static_cast<float>(v2[0]);
			pvVB[iIndex2].vTex.y = static_cast<float>(1.0 - v2[1]);
		}
	}

	// �e�N�X�`�����W
	if (m_dwNumUV && eMapMode == FbxLayerElement::eByControlPoint) {
		// UV�}�b�s���O���[�h��ByControlPoint�̎�
		for (DWORD k = 0; k < m_dwNumUV; ++k) {
			if (eRefMode == FbxLayerElement::eIndexToDirect) {
				index = pUVIndex->GetAt(k);
			} else {
				index = k;
			}
			v2 = pUVDirect->GetAt(index);
			pvVB[k].vTex.x = static_cast<float>(v2[0]);
			pvVB[k].vTex.y = static_cast<float>(1.0 - v2[1]);
		}
	}

	// �}�e���A���ǂݍ���
	FbxNode* pNode = pFbxMesh->GetNode();
	m_dwNumMaterial = pNode->GetMaterialCount();
	m_pMaterial = new TFbxMaterial[m_dwNumMaterial];
	// �}�e���A���̐������C���f�b�N�X�o�b�t�@���쐬
	m_ppIndexBuffer = new ID3D11Buffer*[m_dwNumMaterial];
	ZeroMemory(m_ppIndexBuffer, sizeof(ID3D11Buffer*) * m_dwNumMaterial);
	m_ppIndex = new int*[m_dwNumMaterial];
	ZeroMemory(m_ppIndex, sizeof(int*) * m_dwNumMaterial);
	FbxDouble3 Color, Alpha;
	FbxDouble Factor, AlphaFactor;
	for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		char name[_MAX_PATH];
		strcpy_s(name, _MAX_PATH, pMaterial->GetName());
		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
			FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;
			// ����
			Color = pPhong->Ambient.Get();
			Factor = pPhong->AmbientFactor.Get();
			m_pMaterial[i].Ka.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Ka.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Ka.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Ka.w = 0.0f;	// �e�N�X�`����
			// �g�U���ˌ�
			Color = pPhong->Diffuse.Get();
			Factor = pPhong->DiffuseFactor.Get();
			Alpha = pPhong->TransparentColor.Get();
			AlphaFactor = pPhong->TransparencyFactor.Get();
			m_pMaterial[i].Kd.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Kd.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Kd.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Kd.w = static_cast<float>(1.0 - Alpha[0] * AlphaFactor);
			// ���ʔ��ˌ�
			Color = pPhong->Specular.Get();
			Factor = pPhong->SpecularFactor.Get();
			AlphaFactor = pPhong->Shininess.Get();
			m_pMaterial[i].Ks.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Ks.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Ks.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Ks.w = static_cast<float>(AlphaFactor);
			// �����F
			Color = pPhong->Emissive.Get();
			m_pMaterial[i].Ke.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Ke.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Ke.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Ka.w = 0.0f;	// �e�N�X�`����
		} else if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
			FbxSurfaceLambert* pLambert = (FbxSurfaceLambert*)pMaterial;
			// ����
			Color = pLambert->Ambient.Get();
			Factor = pLambert->AmbientFactor.Get();
			m_pMaterial[i].Ka.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Ka.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Ka.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Ka.w = 0.0f;	// �e�N�X�`����
			// �g�U���ˌ�
			Color = pLambert->Diffuse.Get();
			Factor = pLambert->DiffuseFactor.Get();
			Alpha = pLambert->TransparentColor.Get();
			AlphaFactor = pLambert->TransparencyFactor.Get();
			m_pMaterial[i].Kd.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Kd.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Kd.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Kd.w = static_cast<float>(1.0 - Alpha[0] * AlphaFactor);
			// �����F
			Color = pLambert->Emissive.Get();
			Factor = pLambert->EmissiveFactor.Get();
			m_pMaterial[i].Ke.x = static_cast<float>(Color[0] * Factor);
			m_pMaterial[i].Ke.y = static_cast<float>(Color[1] * Factor);
			m_pMaterial[i].Ke.z = static_cast<float>(Color[2] * Factor);
			m_pMaterial[i].Ka.w = 0.0f;	// �e�N�X�`����
		}
		// �f�B�t���[�Y �e�N�X�`�� (1���̂�)
		FbxProperty lProperty;
		lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		FbxFileTexture* lFileTexture;
		char* pszFName = nullptr;
		size_t uFName = 0;
		for (int j = 0; j < lTextureCount; ++j) {
			lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(j);
			if (!lFileTexture)
				continue;
			FbxUTF8ToAnsi(lFileTexture->GetRelativeFileName(), pszFName, &uFName);
			break;	// �擪��1�����ǂݍ���
		}
		if (pszFName) {
			// �e�N�X�`���쐬
			hr = CreateTextureFromFile(m_pDevice, pszFName, &m_pMaterial[i].pTexture);
			FbxFree(pszFName);
			pszFName = nullptr;
			if (FAILED(hr)) {
				uFName = 0;
				FbxUTF8ToAnsi(lFileTexture->GetFileName(), pszFName, &uFName);
				hr = CreateTextureFromFile(m_pDevice, pszFName, &m_pMaterial[i].pTexture);
				if (FAILED(hr)) {
					char fname[_MAX_PATH];
					char ext[_MAX_EXT];
					_splitpath_s(pszFName, nullptr, 0, nullptr, 0,
						fname, _countof(fname), ext, _countof(ext));
					strcat_s(fname, _countof(fname), ext);
					hr = CreateTextureFromFile(m_pDevice, fname, &m_pMaterial[i].pTexture);
					if (FAILED(hr)) {
						MessageBoxA(0, "�e�N�X�`���ǂݍ��ݎ��s", fname, MB_OK);
						delete[] pvVB;
						return hr;
					}
				}
				if (pszFName) {
					FbxFree(pszFName);
					pszFName = nullptr;
				}
			}
			m_pMaterial[i].Ka.w = 1.0f;	// �e�N�X�`���L
		}
		// �G�~�b�V�u �e�N�X�`�� (1���̂�)
		lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
		lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		for (int j = 0; j < lTextureCount; ++j) {
			lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(j);
			if (!lFileTexture)
				continue;
			FbxUTF8ToAnsi(lFileTexture->GetRelativeFileName(), pszFName, &uFName);
			break;	// �擪��1�����ǂݍ���
		}
		if (pszFName) {
			// �e�N�X�`���쐬
			hr = CreateTextureFromFile(m_pDevice, pszFName, &m_pMaterial[i].pTexEmmisive);
			FbxFree(pszFName);
			pszFName = nullptr;
			if (FAILED(hr)) {
				uFName = 0;
				FbxUTF8ToAnsi(lFileTexture->GetFileName(), pszFName, &uFName);
				hr = CreateTextureFromFile(m_pDevice, pszFName, &m_pMaterial[i].pTexEmmisive);
				if (FAILED(hr)) {
					char fname[_MAX_PATH];
					char ext[_MAX_EXT];
					_splitpath_s(pszFName, nullptr, 0, nullptr, 0,
						fname, _countof(fname), ext, _countof(ext));
					strcat_s(fname, _countof(fname), ext);
					hr = CreateTextureFromFile(m_pDevice, fname, &m_pMaterial[i].pTexEmmisive);
					if (FAILED(hr)) {
						MessageBoxA(0, "�G�~�b�V�u �e�N�X�`���ǂݍ��ݎ��s", fname, MB_OK);
						delete[] pvVB;
						return hr;
					}
				}
				if (pszFName) {
					FbxFree(pszFName);
					pszFName = nullptr;
				}
			}
			m_pMaterial[i].Ke.w = 1.0f;	// �e�N�X�`���L
		}
		// �}�e���A���̐������C���f�b�N�X�o�b�t�@���쐬
		int* pIndex = new int[m_dwNumFace * 3];	// �Ƃ肠�����A���b�V�����̃|���S�����Ń������m�� (�X�̃|���S���O���[�v�͂��Ȃ炸����ȉ��ɂȂ�)
		int iCount = 0;							// ���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�@����ɃC���f�b�N�X�̌������ׂ�
		for (DWORD k = 0; k < m_dwNumFace; ++k) {
			FbxLayerElementMaterial* mat = pFbxMesh->GetLayer(0)->GetMaterials();	// ���C���[��1��������z��
			int matId = mat->GetIndexArray().GetAt(k);
			if (matId == i) {
				if (m_dwNumVert < m_dwNumUV) {
					pIndex[iCount] = pFbxMesh->GetTextureUVIndex(k, 0, FbxLayerElement::eTextureDiffuse);
					pIndex[iCount + 1] = pFbxMesh->GetTextureUVIndex(k, 1, FbxLayerElement::eTextureDiffuse);
					pIndex[iCount + 2] = pFbxMesh->GetTextureUVIndex(k, 2, FbxLayerElement::eTextureDiffuse);
				} else {
					pIndex[iCount] = pFbxMesh->GetPolygonVertex(k, 0);
					pIndex[iCount + 1] = pFbxMesh->GetPolygonVertex(k, 1);
					pIndex[iCount + 2] = pFbxMesh->GetPolygonVertex(k, 2);
				}
				iCount += 3;
			}
		}
		if (iCount > 0) {
			CreateIndexBuffer(iCount * sizeof(int), pIndex, &m_ppIndexBuffer[i]);
			// ���ǉ�
			m_ppIndex[i] = new int[iCount];
			for (int j = 0; j < iCount; ++j) {
				m_ppIndex[i][j] = pIndex[j];
			}
		}
		m_pMaterial[i].dwNumFace = iCount / 3;	// ���̃}�e���A�����̃|���S����		
		delete[] pIndex;
	}

	// ���_����|���S�����t�������邽�߂̋t�����e�[�u������� 
	TPolyTable* PolyTable = new TPolyTable[m_dwNumVert];
	for (DWORD k = 0; k < m_dwNumFace; ++k) {
		for (int m = 0; m < 3; ++m) {
			int i = pFbxMesh->GetPolygonVertex(k, m);
			if (PolyTable[i].nNumRef >= MAX_REF_POLY) {
				delete[] PolyTable;
				delete[] pvVB;
				return E_FAIL;
			}
			PolyTable[i].nPolyIndex[PolyTable[i].nNumRef] = k;
			PolyTable[i].nIndex123[PolyTable[i].nNumRef] = m;
			PolyTable[i].nNumRef++;
		}
	}

	// �X�L����� (�W���C���g�A�E�F�C�g) �ǂݍ���
	ReadSkinInfo(pFbxMesh, pvVB, PolyTable);

	// ���_�o�b�t�@�쐬
	if (m_dwNumVert < m_dwNumUV) m_dwNumVert = m_dwNumUV;
	CD3D11_BUFFER_DESC bd(sizeof(TFbxVertex) * m_dwNumVert, D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pvVB;
	hr = m_pDevice->CreateBuffer(&bd, &sd, &m_pVertexBuffer);

	// ���[�N�폜
	delete[] PolyTable;
	//delete[] pvVB;	�폜

	// ���ǉ�
	m_pVertex = pvVB;

	// �R���X�^���g�o�b�t�@ �{�[���p �쐬
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_BONE);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = m_pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBufferBone);

	return hr;
}

//---------------------------------------------------------------------------------------
// �C���f�b�N�X�o�b�t�@����
//---------------------------------------------------------------------------------------
HRESULT CFbxMesh::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer)
{
	CD3D11_BUFFER_DESC bd(dwSize, D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pIndex;
	sd.SysMemPitch = 0;
	sd.SysMemSlicePitch = 0;
	return m_pDevice->CreateBuffer(&bd, &sd, ppIndexBuffer);
}

//---------------------------------------------------------------------------------------
// ���b�V���`��
//---------------------------------------------------------------------------------------
void CFbxMesh::RenderMesh(EByOpacity byOpacity)
{
	// �萔�̈�X�V
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBuffer0, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_GLOBAL sg;
		XMMATRIX mtxWorld = XMLoadFloat4x4(&m_mFinalWorld);
		XMMATRIX mtxView = XMLoadFloat4x4(&m_mView);
		XMMATRIX mtxProj = XMLoadFloat4x4(&m_mProj);
		sg.mW = XMMatrixTranspose(mtxWorld);
		sg.mWVP = mtxWorld * mtxView * mtxProj;
		sg.mWVP = XMMatrixTranspose(sg.mWVP);
		sg.vEye = XMLoadFloat3(m_pCamera);
		sg.vLightDir = XMLoadFloat3(&m_pLight->m_direction);
		sg.vLd = XMLoadFloat4(&m_pLight->m_diffuse);
		sg.vLa = XMLoadFloat4(&m_pLight->m_ambient);
		sg.vLs = XMLoadFloat4(&m_pLight->m_specular);
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
		m_pDeviceContext->Unmap(m_pConstantBuffer0, 0);
	}
	m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer0);
	m_pDeviceContext->PSSetConstantBuffers(0, 1, &m_pConstantBuffer0);

	// ���_�o�b�t�@���Z�b�g (���_�o�b�t�@��1��)
	UINT stride = sizeof(TFbxVertex);
	UINT offset = 0;
	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// �v���~�e�B�u�`����Z�b�g
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �}�e���A���̐������`��
	for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
		// �g�p����Ă��Ȃ��}�e���A���΍�
		if (m_pMaterial[i].dwNumFace == 0) {
			continue;
		}
		// ���[�U��`�}�e���A��
		TFbxMaterial* pMaterial = &m_pMaterial[i];
		if (m_pMateUsr)
			pMaterial = m_pMateUsr;
		// �����x�ɂ��`��
		switch (byOpacity) {
		case eOpacityOnly:
			if (pMaterial->Kd.w < 1.0f) continue;
			break;
		case eTransparentOnly:
			if (pMaterial->Kd.w >= 1.0f) continue;
			break;
		default:
			break;
		}
		// �C���f�b�N�X�o�b�t�@���Z�b�g
		stride = sizeof(int);
		offset = 0;
		m_pDeviceContext->IASetIndexBuffer(m_ppIndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

		// �}�e���A���̊e�v�f���V�F�[�_�ɓn��
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBuffer1, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
			SHADER_MATERIAL sg;
			sg.vAmbient = XMLoadFloat4(&pMaterial->Ka);
			sg.vDiffuse = XMLoadFloat4(&pMaterial->Kd);
			sg.vSpecular = XMLoadFloat4(&pMaterial->Ks);
			sg.vEmissive = XMLoadFloat4(&pMaterial->Ke);
			memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(sg));
			m_pDeviceContext->Unmap(m_pConstantBuffer1, 0);
		}
		m_pDeviceContext->VSSetConstantBuffers(1, 1, &m_pConstantBuffer1);
		m_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pConstantBuffer1);
		// �e�N�X�`�����V�F�[�_�ɓn��
		m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampleLinear);
		if (m_pMaterial[i].pTexture || m_pMaterial[i].pTexEmmisive) {
			if (m_pMaterial[i].pTexture)
				m_pDeviceContext->PSSetShaderResources(0, 1, &m_pMaterial[i].pTexture);
			if (m_pMaterial[i].pTexEmmisive)
				m_pDeviceContext->PSSetShaderResources(1, 1, &m_pMaterial[i].pTexEmmisive);
		}
		// �{�[�����V�F�[�_�ɓn��
		m_pDeviceContext->VSSetConstantBuffers(2, 1, &m_pConstantBufferBone);
		m_pDeviceContext->PSSetConstantBuffers(2, 1, &m_pConstantBufferBone);
		// �`��
		m_pDeviceContext->DrawIndexed(m_pMaterial[i].dwNumFace * 3, 0, 0);
	}
}

//---------------------------------------------------------------------------------------
// FBX����X�L���֘A�̏���ǂݏo���@
//---------------------------------------------------------------------------------------
HRESULT CFbxMesh::ReadSkinInfo(FbxMesh* pFbxMesh, TFbxVertex* pvVB, TPolyTable* PolyTable)
{
	// ���_���Ƃ̃{�[���C���f�b�N�X�A���_���Ƃ̃{�[���E�F�C�g�A�o�C���h�s��A�|�[�Y�s�� ��4���ڂ𒊏o
	int nSkinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (nSkinCount <= 0) return S_OK;
	m_pBoneTable = new TSkinInfo[nSkinCount];
	m_nNumSkin = nSkinCount;
	int offset = 0;
	for (int nSkin = 0; nSkin < nSkinCount; ++nSkin) {
		TSkinInfo& bt = m_pBoneTable[nSkin];
		// �f�t�H�[�}�擾
		FbxDeformer* pDeformer = pFbxMesh->GetDeformer(nSkin);
		if (!pDeformer) {
			continue;
		}
		FbxSkin* pSkinInfo = static_cast<FbxSkin*>(pDeformer);

		// �{�[���擾
		int nNumBone = pSkinInfo->GetClusterCount();
		bt.ppCluster = new FbxCluster*[nNumBone];
		for (int i = 0; i < nNumBone; ++i) {
			bt.ppCluster[i] = pSkinInfo->GetCluster(i);
		}

		// �ʏ�̏ꍇ (���_����UV���@pvVB�����_�C���f�b�N�X �x�[�X�̏ꍇ)
		if (m_dwNumVert >= m_dwNumUV) {
			// ���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
			for (int i = 0; i < nNumBone; ++i) {
				int nNumIndex = bt.ppCluster[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
				int* piIndex = bt.ppCluster[i]->GetControlPointIndices();
				double* pdWeight = bt.ppCluster[i]->GetControlPointWeights();
				// ���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
				for (int k = 0; k < nNumIndex; ++k) {
					int m;
					for (m = 0; m < 4; ++m) {
						if (pvVB[piIndex[k]].uBoneIndex[m] == i + offset) {
							if (i + offset == 0 && pvVB[piIndex[k]].fBoneWeight[m] <= 0.0f) {
								continue;
							}
							break;
						}
					}
					if (m < 4) continue;	// �ݒ�ςȂ�X�L�b�v
					for (m = 0; m < 4; ++m) {	// 5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��
						if (pdWeight[k] > pvVB[piIndex[k]].fBoneWeight[m]) {
							for (int n = 4 - 1; n > m; --n) {
								pvVB[piIndex[k]].uBoneIndex[n] = pvVB[piIndex[k]].uBoneIndex[n - 1];
								pvVB[piIndex[k]].fBoneWeight[n] = pvVB[piIndex[k]].fBoneWeight[n - 1];
							}
							pvVB[piIndex[k]].uBoneIndex[m] = i + offset;
							pvVB[piIndex[k]].fBoneWeight[m] = static_cast<float>(pdWeight[k]);
							break;
						}
					}
				}
			}
		} else {
			// UV�C���f�b�N�X�x�[�X�̏ꍇ (���_��<UV��)
			int PolyIndex = 0;
			int UVIndex = 0;
			// ���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
			for (int i = 0; i < nNumBone; ++i) {
				int nNumIndex = bt.ppCluster[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
				int* piIndex = bt.ppCluster[i]->GetControlPointIndices();
				double* pdWeight = bt.ppCluster[i]->GetControlPointWeights();
				// ���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
				for (int k = 0; k < nNumIndex; ++k) {
					// ���̒��_���܂�ł���|���S�����ׂĂɁA���̃{�[���ƃE�F�C�g��K�p
					for (int p = 0; p < PolyTable[piIndex[k]].nNumRef; ++p) {
						// ���_�������|���S�������̃|���S����UV�C���f�b�N�X �Ƌt����
						PolyIndex = PolyTable[piIndex[k]].nPolyIndex[p];
						UVIndex = pFbxMesh->GetTextureUVIndex(PolyIndex, PolyTable[piIndex[k]].nIndex123[p], FbxLayerElement::eTextureDiffuse);
						int m;
						for (m = 0; m < 4; ++m) {
							if (pvVB[UVIndex].uBoneIndex[m] == i + offset) {
								if (i + offset == 0 && pvVB[UVIndex].fBoneWeight[m] <= 0.0f) {
									continue;
								}
								break;
							}
						}
						if (m < 4) continue;	// �ݒ�ςȂ�X�L�b�v
						for (m = 0; m < 4; ++m) {	// 5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��
							if (pdWeight[k] > pvVB[UVIndex].fBoneWeight[m]) {
								for (int n = 4 - 1; n > m; --n) {
									pvVB[UVIndex].uBoneIndex[n] = pvVB[UVIndex].uBoneIndex[n - 1];
									pvVB[UVIndex].fBoneWeight[n] = pvVB[UVIndex].fBoneWeight[n - 1];
								}
								pvVB[UVIndex].uBoneIndex[m] = i + offset;
								pvVB[UVIndex].fBoneWeight[m] = static_cast<float>(pdWeight[k]);
								break;
							}
						}
					}
				}
			}
		}

		// �{�[���𐶐�
		bt.nNumBone = nNumBone;
		bt.pBoneArray = new TBone[nNumBone];
		XMFLOAT4X4 m(
			-1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1);
		for (int i = 0; i < nNumBone; ++i) {
			FbxAMatrix mtx;
			bt.ppCluster[i]->GetTransformLinkMatrix(mtx);
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					bt.pBoneArray[i].mBindPose(y, x) = static_cast<float>(mtx.Get(y, x));
				}
			}
			XMStoreFloat4x4(&bt.pBoneArray[i].mBindPose,
				XMLoadFloat4x4(&bt.pBoneArray[i].mBindPose) * XMLoadFloat4x4(&m));
		}

		offset += nNumBone;
	}
	return S_OK;
}

//---------------------------------------------------------------------------------------
// �{�[�������̃|�[�Y�ʒu�ɃZ�b�g����
//---------------------------------------------------------------------------------------
void CFbxMesh::SetNewPoseMatrices(int nFrame)
{
	if (m_nNumSkin <= 0) return;

	XMFLOAT4X4 m(
		-1, 0, 0, 0,
		 0, 1, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1);
	FbxTime time;
	time.SetTime(0, 0, 0, nFrame, 0, 0, FbxTime::eFrames60);	// 60�t���[��/�b �Ɛ��� �����ɂ͏󋵂��Ƃɒ��ׂ�K�v����
	for (int nSkinIndex = 0; nSkinIndex < m_nNumSkin; ++nSkinIndex) {
		TSkinInfo& bt = m_pBoneTable[nSkinIndex];
		for (int i = 0; i < bt.nNumBone; ++i) {
			FbxMatrix mtx = bt.ppCluster[i]->GetLink()->EvaluateGlobalTransform(time);
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					bt.pBoneArray[i].mNewPose(y, x) = static_cast<float>(mtx.Get(y, x));
				}
			}
			XMStoreFloat4x4(&bt.pBoneArray[i].mNewPose,
				XMLoadFloat4x4(&bt.pBoneArray[i].mNewPose) * XMLoadFloat4x4(&m));//FBX�E����W�n�Ȃ̂ł����t�@�␳����	
		}
	}
	// �t���[����i�߂����Ƃɂ��ω������|�[�Y�i�{�[���̍s��j���V�F�[�_�[�ɓn��
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBufferBone, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_BONE sg;
		int j = 0;
		for (int nSkin = 0; nSkin < m_nNumSkin; ++nSkin) {
			for (int i = 0; i < m_pBoneTable[nSkin].nNumBone; ++i) {
				XMFLOAT4X4 mtx = GetCurrentPoseMatrix(nSkin, i);
				if (j >= 256) {
					nSkin = m_nNumSkin;
					break;
				}
				sg.mBone[j++] = XMMatrixTranspose(XMLoadFloat4x4(&mtx));
			}
		}
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_BONE));
		m_pDeviceContext->Unmap(m_pConstantBufferBone, 0);
	}
}

//---------------------------------------------------------------------------------------
// ��(����)�̃|�[�Y�s���Ԃ�
//---------------------------------------------------------------------------------------
XMFLOAT4X4 CFbxMesh::GetCurrentPoseMatrix(int nSkin, int nIndex)
{
	XMMATRIX inv;
	inv = XMMatrixInverse(0, XMLoadFloat4x4(&m_pBoneTable[nSkin].pBoneArray[nIndex].mBindPose));	// FBX�̃o�C���h�|�[�Y�͏����p���i��΍��W�j
	XMFLOAT4X4 ret;
	XMStoreFloat4x4(&ret, inv * XMLoadFloat4x4(&m_pBoneTable[nSkin].pBoneArray[nIndex].mNewPose));	// �o�C���h�|�[�Y�̋t�s��ƃt���[���p���s���������B�Ȃ��A�o�C���h�|�[�Y���̂����ɋt�s��ł���Ƃ���l��������B�iFBX�̏ꍇ�͈Ⴄ���j
	return ret;
}

// ���ǉ�2
//---------------------------------------------------------------------------------------
// �q�̃��b�V�����܂߂����_�����擾
//---------------------------------------------------------------------------------------
int CFbxMesh::GetVertexCount()
{
	int nCount = (int)m_dwNumVert;
	for (DWORD i = 0; i < m_dwNumChild; ++i) {
		nCount += m_ppChild[i]->GetVertexCount();
	}
	return nCount;
}

//---------------------------------------------------------------------------------------
// �q�̃��b�V�����܂߂����_�z����擾
//---------------------------------------------------------------------------------------
int CFbxMesh::GetVertex(TFbxVertex* pVertex, int nCount)
{
	int nCopy = 0;
	if (nCount >= (int)m_dwNumVert) {
		memcpy_s(pVertex, sizeof(TFbxVertex) * nCount,
			m_pVertex, sizeof(TFbxVertex) * m_dwNumVert);
		nCopy += m_dwNumVert;
		pVertex += m_dwNumVert;
		nCount -= m_dwNumVert;
		for (DWORD i = 0; i < m_dwNumChild; ++i) {
			int nVert = m_ppChild[i]->GetVertex(pVertex, nCount);
			nCopy += nVert;
			pVertex += nVert;
			nCount -= nVert;
		}
	}
	return nCopy;
}

//---------------------------------------------------------------------------------------
// �q�̃��b�V�����܂߂��C���f�b�N�X�����擾
//---------------------------------------------------------------------------------------
int CFbxMesh::GetIndexCount()
{
	int nCount = 0;
	for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
		nCount += m_pMaterial[i].dwNumFace * 3;
	}
	for (DWORD i = 0; i < m_dwNumChild; ++i) {
		nCount += m_ppChild[i]->GetIndexCount();
	}
	return nCount;
}

//---------------------------------------------------------------------------------------
// �q�̃��b�V�����܂߂��C���f�b�N�X�z����擾
//---------------------------------------------------------------------------------------
int CFbxMesh::GetIndex(int* pIndex, int nCount, int& nTop)
{
	int nCopy = 0;
	for (DWORD i = 0; i < m_dwNumMaterial; ++i) {
		int nIndex = m_pMaterial[i].dwNumFace * 3;
		if (nCount >= nIndex) {
			for (int j = 0; j < nIndex; ++j) {
				pIndex[j] = m_ppIndex[i][j] + nTop;
			}
			nCopy += nIndex;
			pIndex += nIndex;
			nCount -= nIndex;
		}
	}
	nTop += m_dwNumVert;
	for (DWORD i = 0; i < m_dwNumChild; ++i) {
		int nIndex = m_ppChild[i]->GetIndex(pIndex, nCount, nTop);
		nCopy += nIndex;
		pIndex += nIndex;
		nCount -= nIndex;
	}
	return nCopy;
}
// ���ǉ�2

//---------------------------------------------------------------------------------------
// ���_���W�ŏ��l�ő�l�擾
//---------------------------------------------------------------------------------------
void CFbxMesh::CalcBoundingBox(XMFLOAT3& vMin,
	XMFLOAT3& vMax)
{
	TFbxVertex* pVertex = m_pVertex;
	for (DWORD i = 0; i < m_dwNumVert; ++i, ++pVertex) {
		if (vMin.x > pVertex->vPos.x)
			vMin.x = pVertex->vPos.x;
		if (vMin.y > pVertex->vPos.y)
			vMin.y = pVertex->vPos.y;
		if (vMin.z > pVertex->vPos.z)
			vMin.z = pVertex->vPos.z;
		if (vMax.x < pVertex->vPos.x)
			vMax.x = pVertex->vPos.x;
		if (vMax.y < pVertex->vPos.y)
			vMax.y = pVertex->vPos.y;
		if (vMax.z < pVertex->vPos.z)
			vMax.z = pVertex->vPos.z;
	}
	CFbxMesh** ppChild = m_ppChild;
	for (DWORD i = 0; i < m_dwNumChild; ++i, ++ppChild) {
		(*ppChild)->CalcBoundingBox(vMin, vMax);
	}
}

//---------------------------------------------------------------------------------------
// ���S���W����̍Œ������擾
//---------------------------------------------------------------------------------------
void CFbxMesh::CalcBoundingSphere(XMFLOAT3& vCenter,
	float& fRadius)
{
	TFbxVertex* pVertex = m_pVertex;
	for (DWORD i = 0; i < m_dwNumVert; ++i, ++pVertex) {
		float fDX = pVertex->vPos.x - vCenter.x;
		float fDY = pVertex->vPos.y - vCenter.y;
		float fDZ = pVertex->vPos.z - vCenter.z;
		float fDist = sqrtf(fDX * fDX + fDY * fDY +
			fDZ * fDZ);
		if (fRadius < fDist)
			fRadius = fDist;
	}
	CFbxMesh** ppChild = m_ppChild;
	for (DWORD i = 0; i < m_dwNumChild; ++i, ++ppChild) {
		(*ppChild)->CalcBoundingSphere(vCenter, fRadius);
	}
}

//---------------------------------------------------------------------------------------
// �R���X�g���N�^
//---------------------------------------------------------------------------------------
CFbxModel::CFbxModel()
{
	m_pRootMesh = nullptr;
	m_pDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_pSampleLinear = nullptr;
	m_pConstantBuffer0 = nullptr;
	m_pConstantBuffer1 = nullptr;
	m_pVertexLayout = nullptr;
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	XMStoreFloat4x4(&m_mView, XMMatrixIdentity());
	m_mProj = m_mView;
	m_mWorld = m_mView;
	m_mFinalWorld = m_mView;
	m_pSdkManager = nullptr;
	m_pImporter = nullptr;
	m_pScene = nullptr;
	m_vCamera = XMFLOAT3(0, 0, -1);
	m_nAnimFrame = 0;
	m_nAnimStack = 0;
	m_pMaterial = nullptr;
	m_vCenter = m_vBBox = XMFLOAT3(0, 0, 0);
	m_fRadius = -1.0f;

	m_nAnimStackOld = 0;
	m_nAnimFrameOld = 0;
	m_nBlendFrameMax = 0;
	m_nBlendFrameNow = 0;
}

//---------------------------------------------------------------------------------------
// �f�X�g���N�^
//---------------------------------------------------------------------------------------
CFbxModel::~CFbxModel()
{
	DestroyMesh(m_pRootMesh);
	SAFE_RELEASE(m_pSampleLinear);
	if (m_pSdkManager) m_pSdkManager->Destroy();
}

//---------------------------------------------------------------------------------------
// �K�w���b�V���ǂݍ���
//---------------------------------------------------------------------------------------
HRESULT CFbxModel::LoadRecursive(FbxNode* pNode, CFbxMesh* pFBXMesh)
{
	HRESULT hr = S_OK;

	pFBXMesh->m_pDevice = m_pDevice;
	pFBXMesh->m_pDeviceContext = m_pDeviceContext;
	pFBXMesh->m_pConstantBuffer0 = m_pConstantBuffer0;
	pFBXMesh->m_pConstantBuffer1 = m_pConstantBuffer1;
	pFBXMesh->m_pSampleLinear = m_pSampleLinear;
	pFBXMesh->m_pFBXNode = pNode;

	FbxNodeAttribute *pAttr = pNode->GetNodeAttribute();
	if (pAttr) {
		FbxNodeAttribute::EType type = pAttr->GetAttributeType();
		switch (type) {
		case FbxNodeAttribute::eMesh:
			// ���b�V���쐬
			hr = pFBXMesh->CreateFromFBX(pNode->GetMesh());
			if (FAILED(hr)) {
				MessageBoxW(0, L"���b�V�������G���[", L"CFbxMesh::CreateFromFBX", MB_OK);
				return hr;
			}
			break;
		}
	}

	// �q��������A�q�ōċA
	int iNumChild = pNode->GetChildCount();
	pFBXMesh->m_dwNumChild = iNumChild;
	if (iNumChild > 0) {
		pFBXMesh->m_ppChild = new CFbxMesh*[iNumChild];
		for (int i = 0; i < iNumChild; ++i) {
			pFBXMesh->m_ppChild[i] = new CFbxMesh;
		}
		for (int i = 0; i < iNumChild; ++i) {
			FbxNode* ChildNode = pNode->GetChild(i);
			hr = LoadRecursive(ChildNode, pFBXMesh->m_ppChild[i]);
			if (FAILED(hr)) {
				return hr;
			}
		}
	} else {
		pFBXMesh->m_ppChild = nullptr;
	}
	return hr;
}

//---------------------------------------------------------------------------------------
// ������
//---------------------------------------------------------------------------------------
HRESULT CFbxModel::Init(ID3D11Device* pDevice, ID3D11DeviceContext *pContext, LPCSTR pszFileName)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pContext;

	HRESULT hr = InitShader();
	if (FAILED(hr)) {
		MessageBoxW(0, L"���b�V���p�V�F�[�_�쐬���s", nullptr, MB_OK);
		return hr;
	}

	// �e�N�X�`���p�T���v���쐬
	CD3D11_DEFAULT def;
	CD3D11_SAMPLER_DESC sd(def);
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = m_pDevice->CreateSamplerState(&sd, &m_pSampleLinear);
	if (FAILED(hr)) {
		MessageBoxW(0, L"�e�N�X�`���p�T���v���쐬���s", nullptr, MB_OK);
		return hr;
	}

	char* pszFName = nullptr;
	size_t uFName = 0;
	FbxAnsiToUTF8(pszFileName, pszFName, &uFName);
	hr = CreateFromFBX(pszFName);
	if (pszFName) FbxFree(pszFName);
	if (FAILED(hr)) {
		MessageBoxW(0, L"���b�V���쐬���s", nullptr, MB_OK);
		return hr;
	}

	return hr;
}

//---------------------------------------------------------------------------------------
// �V�F�[�_�ǂݍ���
//---------------------------------------------------------------------------------------
HRESULT CFbxModel::InitShader()
{
	// �V�F�[�_�ǂݍ���
	HRESULT hr = LoadShader("FbxModelVertex", "FbxModelPixel",
		&m_pVertexShader, &m_pVertexLayout, &m_pPixelShader);
	if (FAILED(hr)) {
		MessageBoxW(0, L"hlsl�ǂݍ��ݎ��s", nullptr, MB_OK);
		return hr;
	}

	// �R���X�^���g�o�b�t�@�쐬 �ϊ��s��n���p
	D3D11_BUFFER_DESC cb;
	ZeroMemory(&cb, sizeof(cb));
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_GLOBAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = m_pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer0);
	if (FAILED(hr)) {
		return hr;
	}

	// �R���X�^���g�o�b�t�@�쐬 �}�e���A���n���p
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(SHADER_MATERIAL);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	hr = m_pDevice->CreateBuffer(&cb, nullptr, &m_pConstantBuffer1);

	return hr;
}

//---------------------------------------------------------------------------------------
// FBX������
//---------------------------------------------------------------------------------------
HRESULT CFbxModel::InitFBX(LPCSTR szFileName)
{
	m_pSdkManager = FbxManager::Create();
	m_pImporter = FbxImporter::Create(m_pSdkManager, "MyImporter");
	m_pImporter->Initialize(szFileName);
	m_pScene = FbxScene::Create(m_pSdkManager, "MyScene");
	m_pImporter->Import(m_pScene);

	// DirectX�n
	//FbxAxisSystem::DirectX.ConvertScene(m_pScene);

	// �O�p�`��
	FbxGeometryConverter lGeomConverter(m_pSdkManager);
	lGeomConverter.Triangulate(m_pScene, true);

	// �A�j���[�V�������
	m_pScene->FillAnimStackNameArray(m_strAnimStackName);
	if (m_strAnimStackName.GetCount() > 0) {
		SetAnimStack(0);
	}
	
	return S_OK;
}

//---------------------------------------------------------------------------------------
// FBX���烁�b�V�����쐬
//---------------------------------------------------------------------------------------
HRESULT CFbxModel::CreateFromFBX(LPCSTR szFileName)
{
	// FBX���[�_������
	HRESULT hr = InitFBX(szFileName);
	if (FAILED(hr)) {
		MessageBoxW(0, L"FBX���[�_���������s", nullptr, MB_OK);
		return hr;
	}

	char szDir[_MAX_DIR];
	char szCurrentDir[_MAX_PATH];
	// �t�H���_���𒊏o
	_splitpath_s(szFileName, nullptr, 0, szDir, _countof(szDir),
		nullptr, 0, nullptr, 0);
	// �J�����g�f�B���N�g����ύX
	if (szDir[0]) {
		GetCurrentDirectoryA(_MAX_PATH, szCurrentDir);
		SetCurrentDirectoryA(szDir);
	}

	FbxNode* pNode = m_pScene->GetRootNode();
	m_pRootMesh = new CFbxMesh;
	hr = LoadRecursive(pNode, m_pRootMesh);

	// �J�����g�f�B���N�g�������ɖ߂�
	if (szDir[0])
		SetCurrentDirectoryA(szCurrentDir);

	return hr;
}

//---------------------------------------------------------------------------------------
// �����_�����O
//---------------------------------------------------------------------------------------
void CFbxModel::Render(XMFLOAT4X4& mWorld, XMFLOAT4X4& mView, XMFLOAT4X4& mProj, EByOpacity byOpacity)
{
	m_mWorld = mWorld;
	m_mView = mView;
	m_mProj = mProj;
	// �g�p����V�F�[�_�[�̓o�^	
	m_pDeviceContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pDeviceContext->PSSetShader(m_pPixelShader, nullptr, 0);
	// ���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pDeviceContext->IASetInputLayout(m_pVertexLayout);

	RecursiveRender(m_pRootMesh, byOpacity);
}

//---------------------------------------------------------------------------------------
// ���b�V���`��
//---------------------------------------------------------------------------------------
void CFbxModel::RenderMesh(CFbxMesh* pMesh, EByOpacity byOpacity)
{
	pMesh->m_mView = m_mView;
	pMesh->m_mProj = m_mProj;
	pMesh->m_pLight = &m_light;
	pMesh->m_pCamera = &m_vCamera;
	pMesh->m_pMateUsr = m_pMaterial;
	pMesh->RenderMesh(byOpacity);
}

//---------------------------------------------------------------------------------------
// �K�w�A�j���[�V�����Đ�
//---------------------------------------------------------------------------------------
void CFbxModel::RecursiveRender(CFbxMesh* pMesh, EByOpacity byOpacity)
{
	// ���̏u�Ԃ́A�����̎p���s��𓾂�
	FbxTime time;
	time.SetTime(0, 0, 0, m_nAnimFrame, 0, 0, FbxTime::eFrames60);//���̃t���[���ɑ΂��鎞��
	FbxAMatrix mCurrent = pMesh->m_pFBXNode->EvaluateGlobalTransform(time);
	
	// �s��R�s�[ (DirectX �� FBX)
	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			pMesh->m_mFBXOrientation(y, x) =
				static_cast<float>(mCurrent.Get(y, x));
		}
	}

	// �e�̎p���s��ƍ���
	pMesh->m_mParentOrientation = m_mWorld;
	XMStoreFloat4x4(&pMesh->m_mFinalWorld,
		XMLoadFloat4x4(&pMesh->m_mFBXOrientation) * XMLoadFloat4x4(&pMesh->m_mParentOrientation));
	RenderMesh(pMesh, byOpacity);
	for (DWORD i = 0; i < pMesh->m_dwNumChild; ++i) {
		RecursiveRender(pMesh->m_ppChild[i], byOpacity);
	}
}

//---------------------------------------------------------------------------------------
// �S���b�V���j��
//---------------------------------------------------------------------------------------
void CFbxModel::DestroyMesh(CFbxMesh* pMesh)
{
	if (!pMesh) return;
	for (DWORD i = 0; i < pMesh->m_dwNumChild; ++i) {
		if (pMesh->m_ppChild[i])
			DestroyMesh(pMesh->m_ppChild[i]);
	}
	delete pMesh;
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �t���[���X�V
//---------------------------------------------------------------------------------------
void CFbxModel::SetAnimFrame(int nFrame)
{
	m_nAnimFrame = nFrame;
	SetNewPoseMatrices(m_pRootMesh, nFrame);
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �t���[���X�V
//---------------------------------------------------------------------------------------
void CFbxModel::SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame)
{
	if (pMesh) {
		pMesh->SetNewPoseMatrices(nFrame);
		for (DWORD i = 0; i < pMesh->m_dwNumChild; ++i)
			SetNewPoseMatrices(pMesh->m_ppChild[i], nFrame);
	}
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �t���[�����擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetMaxAnimFrame()
{
	return static_cast<int>(m_tStop.GetFrameCount(FbxTime::eFrames60));
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �X�^�[�g�t���[�����擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetMinAnimFrame()
{
	return static_cast<int>(m_tStart.GetFrameCount(FbxTime::eFrames60));
}

//---------------------------------------------------------------------------------------
// �A�j���[�V�������擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetMaxAnimStack()
{
	return m_strAnimStackName.GetCount();
}

//---------------------------------------------------------------------------------------
// �A�j���[�V�����؊�
//---------------------------------------------------------------------------------------
void CFbxModel::SetAnimStack(int nAnimStack)
{
	int nStackCount = GetMaxAnimStack();
	if (nAnimStack < 0 || nAnimStack >= nStackCount)
		return;
	m_nAnimStack = nAnimStack;
	FbxAnimStack *AnimationStack = m_pScene->FindMember<FbxAnimStack>(m_strAnimStackName[m_nAnimStack]->Buffer());
	m_pScene->SetCurrentAnimationStack(AnimationStack);
	FbxTakeInfo *takeInfo = m_pScene->GetTakeInfo(*m_strAnimStackName[m_nAnimStack]);
	m_tStart = takeInfo->mLocalTimeSpan.GetStart();
	m_tStop = takeInfo->mLocalTimeSpan.GetStop();
}

//---------------------------------------------------------------------------------------
// �����ݒ�
//---------------------------------------------------------------------------------------
void CFbxModel::SetLight(CFbxLight& light)
{
	m_light = light;
}

//---------------------------------------------------------------------------------------
// ���_���W�ݒ�
//---------------------------------------------------------------------------------------
void CFbxModel::SetCamera(DirectX::XMFLOAT3& vCamera)
{
	m_vCamera = vCamera;
}

//---------------------------------------------------------------------------------------
// �}�e���A���ݒ�
//---------------------------------------------------------------------------------------
void CFbxModel::SetMaterial(TFbxMaterial* pMaterial)
{
	if (pMaterial) {
		m_material = *pMaterial;
		m_pMaterial = &m_material;
	} else {
		m_pMaterial = nullptr;
	}
}

// ���ǉ�2
//---------------------------------------------------------------------------------------
// ���_���擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetVertexCount()
{
	if (m_pRootMesh) {
		return m_pRootMesh->GetVertexCount();
	}
	return 0;
}

//---------------------------------------------------------------------------------------
// ���_�z��擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetVertex(TFbxVertex* pVertex, int nCount)
{
	if (m_pRootMesh) {
		return m_pRootMesh->GetVertex(pVertex, nCount);
	}
	return 0;
}

//---------------------------------------------------------------------------------------
// �C���f�b�N�X���̎擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetIndexCount()
{
	if (m_pRootMesh) {
		return m_pRootMesh->GetIndexCount();
	}
	return 0;
}

//---------------------------------------------------------------------------------------
// �C���f�b�N�X�z��̎擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetIndex(int* pIndex, int nCount)
{
	if (m_pRootMesh) {
		int nTop = 0;
		return m_pRootMesh->GetIndex(pIndex, nCount, nTop);
	}
	return 0;
}

//---------------------------------------------------------------------------------------
// ���E�{�b�N�X�A���E���Z�o
//---------------------------------------------------------------------------------------
void CFbxModel::CalcBoundingSphere()
{
	XMFLOAT3 vMin, vMax;
	vMin.x = vMin.y = vMin.z = FLT_MAX;
	vMax.x = vMax.y = vMax.z = -FLT_MAX;
	if (m_pRootMesh) {
		m_pRootMesh->CalcBoundingBox(vMin, vMax);
		m_vCenter.x = (vMax.x + vMin.x) / 2.0f;
		m_vCenter.y = (vMax.y + vMin.y) / 2.0f;
		m_vCenter.z = (vMax.z + vMin.z) / 2.0f;
		m_vBBox.x = (vMax.x - vMin.x) / 2.0f;
		m_vBBox.y = (vMax.y - vMin.y) / 2.0f;
		m_vBBox.z = (vMax.z - vMin.z) / 2.0f;

		m_fRadius = -1.0f;
		m_pRootMesh->CalcBoundingSphere(m_vCenter,
			m_fRadius);
	}
}

//---------------------------------------------------------------------------------------
// ���E�{�b�N�X(���E��)���S���W�擾
//---------------------------------------------------------------------------------------
XMFLOAT3& CFbxModel::GetCenter()
{
	if (m_fRadius < 0.0f) {
		CalcBoundingSphere();
	}
	return m_vCenter;
}

//---------------------------------------------------------------------------------------
// ���E�{�b�N�X �T�C�Y(�c�����s���̔���)�擾
//---------------------------------------------------------------------------------------
XMFLOAT3& CFbxModel::GetBBox()
{
	if (m_fRadius < 0.0f) {
		CalcBoundingSphere();
	}
	return m_vBBox;
}

//---------------------------------------------------------------------------------------
// ���E�����a�擾
//---------------------------------------------------------------------------------------
float CFbxModel::GetRadius()
{
	if (m_fRadius < 0.0f) {
		CalcBoundingSphere();
	}
	return m_fRadius;
}


CFbxMesh* CFbxModel::GetRootMesh()
{
	return m_pRootMesh;
}

// ����֐�

//---------------------------------------------------------------------------------------
// ���f���̃e�N�X�`���ݒ�
//---------------------------------------------------------------------------------------
void CFbxModel::SetTexture(ID3D11ShaderResourceView* pTex)
{
	m_pRootMesh->SetTexture(pTex);
}
//---------------------------------------------------------------------------------------
// ���f���̃e�N�X�`���ݒ�
//---------------------------------------------------------------------------------------
void CFbxMesh::SetTexture(ID3D11ShaderResourceView* pTex)
{
	for (DWORD i = 0; i < m_dwNumChild; ++i) {
		m_ppChild[i]->SetTexture(pTex);
	}
	for (DWORD i = 0; i < m_dwNumMaterial; ++i)
	{
		m_pMaterial[i].pTexture = pTex;
	}

}

//---------------------------------------------------------------------------------------
// �{�[�������̃|�[�Y�ʒu�ɃZ�b�g����
//---------------------------------------------------------------------------------------
void CFbxMesh::SetNewPoseMatrices(int nFrame,int nOldFrame,float blendPar)
{
	if (m_nNumSkin <= 0) return;
	if (blendPar > 1.0f)blendPar = 1.0f;
	if (blendPar < 0.0f)blendPar = 0.0f;

	XMFLOAT4X4 m(
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);
	FbxTime time,timeOld;
	time.SetTime(0, 0, 0, nFrame, 0, 0, FbxTime::eFrames60);	// 60�t���[��/�b �Ɛ��� �����ɂ͏󋵂��Ƃɒ��ׂ�K�v����
	timeOld.SetTime(0, 0, 0, nOldFrame, 0, 0, FbxTime::eFrames60);
	for (int nSkinIndex = 0; nSkinIndex < m_nNumSkin; ++nSkinIndex) {
		TSkinInfo& bt = m_pBoneTable[nSkinIndex];
		for (int i = 0; i < bt.nNumBone; ++i) {
			FbxMatrix mtx = bt.ppCluster[i]->GetLink()->EvaluateGlobalTransform(time);
			FbxMatrix mtxOld = bt.ppCluster[i]->GetLink()->EvaluateGlobalTransform(timeOld);
			FbxMatrix mtxres = (mtx * blendPar) + (mtxOld * (1.0f - blendPar));
			for (int x = 0; x < 4; ++x) {
				for (int y = 0; y < 4; ++y) {
					bt.pBoneArray[i].mNewPose(y, x) = static_cast<float>(mtxres.Get(y, x));
				}
			}
			XMStoreFloat4x4(&bt.pBoneArray[i].mNewPose,
				XMLoadFloat4x4(&bt.pBoneArray[i].mNewPose) * XMLoadFloat4x4(&m));//FBX�E����W�n�Ȃ̂ł����t�@�␳����	
		}
	}
	// �t���[����i�߂����Ƃɂ��ω������|�[�Y�i�{�[���̍s��j���V�F�[�_�[�ɓn��
	D3D11_MAPPED_SUBRESOURCE pData;
	if (SUCCEEDED(m_pDeviceContext->Map(m_pConstantBufferBone, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData))) {
		SHADER_BONE sg;
		int j = 0;
		for (int nSkin = 0; nSkin < m_nNumSkin; ++nSkin) {
			for (int i = 0; i < m_pBoneTable[nSkin].nNumBone; ++i) {
				XMFLOAT4X4 mtx = GetCurrentPoseMatrix(nSkin, i);
				if (j >= 256) {
					nSkin = m_nNumSkin;
					break;
				}
				sg.mBone[j++] = XMMatrixTranspose(XMLoadFloat4x4(&mtx));
			}
		}
		memcpy_s(pData.pData, pData.RowPitch, (void*)&sg, sizeof(SHADER_BONE));
		m_pDeviceContext->Unmap(m_pConstantBufferBone, 0);
	}
}

//---------------------------------------------------------------------------------------
// �A�j���[�V�����؊�
//---------------------------------------------------------------------------------------
void CFbxModel::SetAnimStackBlend(int nAnimStack,int BlendFrame)
{
	int nStackCount = GetMaxAnimStack();
	if (nAnimStack < 0 || nAnimStack >= nStackCount)
		return;
	m_nBlendFrameMax = BlendFrame;
	m_nBlendFrameNow = 0;
	m_tStartOld = m_tStart;
	m_tStopOld = m_tStop;
	m_nAnimStack = nAnimStack;
	FbxAnimStack *AnimationStack = m_pScene->FindMember<FbxAnimStack>(m_strAnimStackName[m_nAnimStack]->Buffer());
	m_pScene->SetCurrentAnimationStack(AnimationStack);
	FbxTakeInfo *takeInfo = m_pScene->GetTakeInfo(*m_strAnimStackName[m_nAnimStack]);
	m_tStart = takeInfo->mLocalTimeSpan.GetStart();
	m_tStop = takeInfo->mLocalTimeSpan.GetStop();
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �t���[���X�V
//---------------------------------------------------------------------------------------
void CFbxModel::SetAnimFrame(int nFrame,int nOldFrame)
{
	m_nAnimFrame = nFrame;
	m_nAnimFrameOld = nOldFrame;
	SetNewPoseMatrices(m_pRootMesh, nFrame,nOldFrame);
}

//---------------------------------------------------------------------------------------
// �A�j���[�V���� �t���[���X�V
//---------------------------------------------------------------------------------------
void CFbxModel::SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame, int nOldFrame)
{
	if (pMesh) {
		if(m_nBlendFrameMax > 0)	// �u�����h����A�j���[�V�������o�^����Ă��邩
			pMesh->SetNewPoseMatrices(nFrame,nOldFrame,(float)m_nBlendFrameNow / m_nBlendFrameMax);
		else
			pMesh->SetNewPoseMatrices(nFrame);
		for (DWORD i = 0; i < pMesh->m_dwNumChild; ++i)
			SetNewPoseMatrices(pMesh->m_ppChild[i], nFrame,nOldFrame);
	}
}


//---------------------------------------------------------------------------------------
// �u�����h�p�O��A�j���[�V���� �t���[�����擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetMaxAnimFrameOld()
{
	return static_cast<int>(m_tStopOld.GetFrameCount(FbxTime::eFrames60));
}

//---------------------------------------------------------------------------------------
// �u�����h�p�O��A�j���[�V���� �X�^�[�g�t���[�����擾
//---------------------------------------------------------------------------------------
int CFbxModel::GetMinAnimFrameOld()
{
	return static_cast<int>(m_tStartOld.GetFrameCount(FbxTime::eFrames60));
}

void CFbxModel::SetBlendFrameAdd(int val)
{
	m_nBlendFrameNow += val;
}
//=======================================================================================
//	End of File
//=======================================================================================

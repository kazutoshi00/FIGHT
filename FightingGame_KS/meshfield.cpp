#include "meshfield.h"
#include "mesh.h"
#include "Texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_FILENAME	"data/texture/field003.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MESH		g_meshfield;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeshField(XMFLOAT3 pos, XMFLOAT3 rot,
	int nNumBlockX, int nNumBlockZ, float fSizeBlockX, float fSizeBlockZ)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// �e�N�X�`�� �}�g���b�N�X������
	XMStoreFloat4x4(&g_meshfield.mtxTexture, XMMatrixIdentity());

	// �ʒu�A�����̏����ݒ�
	g_meshfield.pos = pos;
	g_meshfield.rot = rot;
	g_meshfield.size = XMFLOAT3(1,1,1);

	// �e�N�X�`���̓ǂݍ���
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
							   TEXTURE_FILENAME,		// �t�@�C���̖��O
							   &g_meshfield.pTexture);	// �ǂݍ��ރ������[
	if (FAILED(hr))
		return hr;

	// ���_���̐ݒ�
	g_meshfield.nNumVertex = (nNumBlockX + 1) * (nNumBlockZ + 1);

	// �C���f�b�N�X���̐ݒ�
	g_meshfield.nNumIndex = (nNumBlockX + 1) * 2 * nNumBlockZ + (nNumBlockZ - 1) * 2;

	// �|���S�����̐ݒ�
	int nNumPolygon = nNumBlockX * nNumBlockZ * 2 + (nNumBlockZ - 1) * 4;

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[g_meshfield.nNumVertex];

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
	int* pIndexWk = new int[g_meshfield.nNumIndex];

	// ���_�o�b�t�@�̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;
#if 0
	const float texSizeX = 1.0f / nNumBlockX;
	const float texSizeZ = 1.0f / nNumBlockZ;
#else
	const float texSizeX = 1.0f;
	const float texSizeZ = 1.0f;
#endif

	for (int nCntVtxZ = 0; nCntVtxZ < nNumBlockZ + 1; ++nCntVtxZ) {
		const float ANGLE = XM_2PI / nNumBlockX;

		for (int nCntVtxX = 0; nCntVtxX < nNumBlockX + 1; ++nCntVtxX) {
			// ���_���W�̐ݒ�
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.x = -(nNumBlockX / 2.0f) * fSizeBlockX + nCntVtxX * fSizeBlockX;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.y = 0.0f;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].vtx.z = (nNumBlockZ / 2.0f) * fSizeBlockZ - nCntVtxZ * fSizeBlockZ;

			// �@���̐ݒ�
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].nrm = XMFLOAT3(0.0f, 1.0, 0.0f);
			
			// ���ˌ��̐ݒ�
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
			pVtx[nCntVtxZ * (nNumBlockX + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxZ;
		}
	}

	// �C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
	int* pIdx = pIndexWk;

	int nCntIdx = 0;
	for (int nCntVtxZ = 0; nCntVtxZ < nNumBlockZ; ++nCntVtxZ) {
		if (nCntVtxZ > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockX + 1);
			++nCntIdx;
		}

		for (int nCntVtxX = 0; nCntVtxX < nNumBlockX + 1; ++nCntVtxX) {
			pIdx[nCntIdx] = (nCntVtxZ + 1) * (nNumBlockX + 1) + nCntVtxX;
			++nCntIdx;
			pIdx[nCntIdx] = nCntVtxZ * (nNumBlockX + 1) + nCntVtxX;
			++nCntIdx;
		}

		if (nCntVtxZ < nNumBlockZ - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			pIdx[nCntIdx] = nCntVtxZ * (nNumBlockX + 1) + nNumBlockX;
			++nCntIdx;
		}
	}

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	hr = MakeMeshVertex(pDevice, &g_meshfield, pVertexWk, pIndexWk);

	// �ꎞ�z����
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMeshField(void)
{
	// ���b�V�����
	ReleaseMesh(&g_meshfield);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMeshField(void)
{
	// ���b�V���X�V
	UpdateMesh(&g_meshfield);
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeshField(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// ���b�V���`��
	DrawMesh(pDeviceContext, &g_meshfield,true);
}

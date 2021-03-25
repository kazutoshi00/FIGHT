#pragma once
#include "main.h"
#include "Object.h"
#include "mesh.h"
class COBB: public CObject
{
protected:
	XMVECTOR m_NormaDirect[3];   // 方向ベクトル
	float m_fLength[3];             // 各軸方向の長さ

	MESH* m_pMesh;
	void SetMesh();

	bool m_bUse;
	bool m_bDispHitbox;		// ヒットボックスを描画するかどうか

public:
	XMVECTOR GetDirect(int elem);   // 指定軸番号の方向ベクトルを取得
	float GetLen_W(int elem);          // 指定軸方向の長さを取得
	XMFLOAT3 GetPos_W();             // 位置を取得

public:
	COBB();
	~COBB();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void SetSize(XMFLOAT3);
	void SetPos(XMFLOAT3);
	void SetRot(XMFLOAT3);
	XMFLOAT3 GetSize();
	XMFLOAT3 GetPos();
	XMFLOAT3 GetRot();
	bool IsUse() { return m_bUse; }						// ヒットボックスがアクティブかどうか
	void SetUse(bool flg) { m_bUse = flg; }				// ヒットボックスのアクティブの切り替え
	void SetDisp(bool disp) { m_bDispHitbox = disp; }	// ヒットボックスの描画の切り替え

	XMFLOAT3 GetWorldSize() { return CObject::GetWorldScale(); }

};
bool ColObbs(COBB &obb1, COBB &obb2);



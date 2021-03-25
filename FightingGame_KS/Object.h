#pragma once
#include "main.h"
class CObject
{
private:
	XMFLOAT3 m_worldposition;	// ポジション(親を考慮したworldPos)
	XMFLOAT3 m_worldrotation;	// 回転(親を考慮したworldRot)
	XMFLOAT3 m_worldscale;		// 拡大率(親を考慮したworldScale)
	bool m_bUpdated;		// 更新されたかどうか
	CObject* m_pChild;		// 子オブジェクト
	CObject* m_pBrotherNext;	// 兄弟オブジェクト(子オブジェクトリストの次ポインタ)


protected:
	CObject* m_pParent;	// 親オブジェクト
	CObject* m_pNext;	// 次のオブジェクト
	CObject* m_pBack;	// 前のオブジェクト

	XMFLOAT4X4 m_world;		// ワールド マトリックス
	XMFLOAT4X4 m_worldRotWork;	// 回転を求めるために必要な行列の格納場所
	XMFLOAT3 m_localposition;	// ポジション
	XMFLOAT3 m_localrotation;	// 回転
	XMFLOAT3 m_localscale;		// 拡大率(大きさ)

	XMFLOAT3& GetWorldPosition() { return m_worldposition; }
	XMFLOAT3& GetWorldRotation() { return m_worldrotation; }
	XMFLOAT3& GetWorldScale() { return m_worldscale; }
	bool IsUpdated() { return m_bUpdated; }

public:
	CObject();
	~CObject();

	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	void UpdateMatrix();

	virtual void SetSize(XMFLOAT3 size);
	virtual void SetPos(XMFLOAT3 pos);
	virtual void SetRot(XMFLOAT3 rot);
	XMFLOAT4X4& GetWorldMatrix() { return m_world; };
	XMFLOAT4X4& GetWorldRotWorkMatrix() { return m_worldRotWork; };
	void SetNext(CObject* pNext) { m_pNext = pNext; }		// 次のオブジェクトのポインタをセットする(リスト構造)
	void SetBack(CObject*pBack) { m_pBack = pBack; }		// 前のオブジェクトのポインタをセットする(リスト構造)
	CObject* GetNext() { return m_pNext; }			// 次のオブジェクトのポインタ(リスト構造)
	CObject* GetBack() { return m_pBack; }			// 次のオブジェクトのポインタ(リスト構造)
	
	void SetParent(CObject*);	// 親オブジェクトのセット
	void DeleteChild(CObject*);	// 引数のオブジェクトを子供に持つオブジェクトから、その情報を削除する
	CObject* GetParent();	// 親オブジェクトの取得
};

class CObjectManager
{
private:
	static CObjectManager m_ObjectManager;
	CObject* m_pRootObj;
public:
	static CObjectManager* Get() { return &m_ObjectManager; }
	void Init();
	void Uninit();
	void Update();
	void Draw();
	void AddObject(CObject*);
	void DeleteObject(CObject*);
};

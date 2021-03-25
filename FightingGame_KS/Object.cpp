#include "Object.h"

// 静的メンバ変数
CObjectManager CObjectManager::m_ObjectManager;
// ***オブジェクトマネージャークラス**************************************************
void CObjectManager::Init()
{

}
void CObjectManager::Uninit()
{
	CObject* pNextObj = nullptr;
	for (CObject* pObj = m_pRootObj; pObj != nullptr; pObj = pNextObj)
	{
		pNextObj = pObj->GetNext();
		pObj->Uninit();
	}
}
void CObjectManager::Update()
{
	CObject* pNextObj = nullptr;

	for (CObject* pObj = m_pRootObj; pObj != nullptr; pObj = pNextObj)
	{
		pNextObj = pObj->GetNext();
		pObj->Update();
	}

}
void CObjectManager::Draw()
{
	CObject* pNextObj = nullptr;
	for (CObject* pObj = m_pRootObj; pObj != nullptr; pObj = pNextObj)
	{
		pNextObj = pObj->GetNext();
		pObj->Draw();
	}
}
void CObjectManager::AddObject(CObject* AddObj)
{
	if (m_pRootObj == nullptr)
	{
		m_pRootObj = AddObj;
	}
	else
	{
		CObject* LastObj = m_pRootObj;
		while (LastObj->GetNext() != nullptr)
		{
			LastObj = LastObj->GetNext();
		}
		LastObj->SetNext(AddObj);
		AddObj->SetBack(LastObj);
	}
	//AddObj->Init();
}

void CObjectManager::DeleteObject(CObject* DelObj)
{
	if (DelObj == nullptr)return;
	CObject* pNextObj = nullptr;
	// 削除するオブジェクトを親に持つオブジェクトがある場合に、そのオブジェクトの親を削除するオブジェクトの親に変更する
	for (CObject* pObj = m_pRootObj; pObj != nullptr; pObj = pNextObj)
	{
		pNextObj = pObj->GetNext();
		if (pObj->GetParent() == DelObj)
		{
			pObj->SetParent(DelObj->GetParent());
		}
	}
	// 先頭のオブジェクトだったら先頭を次のオブジェクトに変更する
	if (m_pRootObj == DelObj)
	{
		m_pRootObj = m_pRootObj->GetNext();
		if (m_pRootObj != nullptr)
		{
			m_pRootObj->SetBack(nullptr);
		}
		return;
	}
	pNextObj = nullptr;
	for (CObject* pObj = m_pRootObj->GetNext(); pObj != nullptr; pObj = pNextObj)
	{
		pNextObj = pObj->GetNext();
		if (pObj == DelObj)
		{
			CObject* back = pObj->GetBack();
			back->SetNext(pNextObj);
			if(pNextObj != nullptr)
				pNextObj->SetBack(back);
			//DelObj->Uninit();
			return;
		}
	}
}







// ***オブジェクトクラス**************************************************************
CObject::CObject()
{
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
	m_pParent = nullptr;
	m_pNext = nullptr;
	m_pBack = nullptr;
	m_worldposition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_worldrotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_worldscale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_localposition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_localrotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_localscale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_bUpdated = false;
	m_pChild = nullptr;
	m_pBrotherNext = nullptr;
}

CObject::~CObject()
{
}
HRESULT CObject::Init()
{
	return S_OK;
}
void CObject::Uninit()
{

}
void CObject::Update()
{
	if (m_pParent)m_pParent->Update();	// 親がいる場合、親の更新処理を先にする
	if (m_bUpdated)return;				// すでに更新済みの場合、関数を抜ける
	XMMATRIX mtxWorld, mtxParentWorld, mtxWorldRotWork, mtxParentWorldRotWork, mtxRot, mtxTranslate, mtxScale;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();
	mtxParentWorld = XMMatrixIdentity();
	mtxWorldRotWork = XMMatrixIdentity();
	mtxParentWorldRotWork = XMMatrixIdentity();
	if (m_pParent)
	{
		mtxParentWorld = XMLoadFloat4x4(&m_pParent->GetWorldMatrix());
		mtxParentWorldRotWork = XMLoadFloat4x4(&m_pParent->GetWorldRotWorkMatrix());
	}
	// 拡縮を反映
	mtxScale = XMMatrixScaling(m_localscale.x, m_localscale.y, m_localscale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_localrotation.x), XMConvertToRadians(m_localrotation.y), XMConvertToRadians(m_localrotation.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	mtxWorldRotWork = XMMatrixMultiply(mtxWorldRotWork, mtxRot);
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_localposition.x, m_localposition.y, m_localposition.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	// 親のワールドマトリックスを掛け合わせる
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxParentWorld);
	mtxWorldRotWork = XMMatrixMultiply(mtxWorldRotWork, mtxParentWorldRotWork);
	// ワールドマトリックスの設定
	XMStoreFloat4x4(&m_world, mtxWorld);
	XMStoreFloat4x4(&m_worldRotWork, mtxWorldRotWork);
	// ワールド位置をセット
	m_worldposition.x = m_world._41;
	m_worldposition.y = m_world._42;
	m_worldposition.z = m_world._43;
	// ワールド回転をセット
	XMFLOAT3 WorldRot;
	if (mtxWorldRotWork.r[2].m128_f32[1] == 1.0f)
	{
		WorldRot.x = XM_PI / 2;
		WorldRot.y = 0;
		WorldRot.z = atan2f(mtxWorldRotWork.r[1].m128_f32[0], mtxWorldRotWork.r[0].m128_f32[0]);
	}
	else if (mtxWorldRotWork.r[2].m128_f32[1] == -1.0f)
	{
		WorldRot.x = -XM_PI / 2;
		WorldRot.y = 0;
		WorldRot.z = atan2f(mtxWorldRotWork.r[1].m128_f32[0], mtxWorldRotWork.r[0].m128_f32[0]);
	}
	else
	{
		WorldRot.x = asinf(-mtxWorldRotWork.r[2].m128_f32[1]);
		WorldRot.y = atan2f(mtxWorldRotWork.r[2].m128_f32[0], mtxWorldRotWork.r[2].m128_f32[2]);
		WorldRot.z = atan2f(mtxWorldRotWork.r[0].m128_f32[1], mtxWorldRotWork.r[1].m128_f32[1]);
	}
	m_worldrotation = XMFLOAT3(XMConvertToDegrees(WorldRot.x), XMConvertToDegrees(WorldRot.y), XMConvertToDegrees(WorldRot.z));
	XMFLOAT3 parscl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	if (m_pParent)
		parscl = m_pParent->GetWorldScale();
	m_worldscale = XMFLOAT3(m_localscale.x * parscl.x, m_localscale.y * parscl.y, m_localscale.z * parscl.z);
	// 更新済みにする
	m_bUpdated = true;
}
void CObject::Draw()
{
	m_bUpdated = false;
}

void CObject::UpdateMatrix()
{
	XMMATRIX mtxWorld, mtxParentWorld, mtxWorldRotWork, mtxParentWorldRotWork, mtxRot, mtxTranslate, mtxScale;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();
	mtxParentWorld = XMMatrixIdentity();
	mtxWorldRotWork = XMMatrixIdentity();
	mtxParentWorldRotWork = XMMatrixIdentity();
	if (m_pParent)
	{
		mtxParentWorld = XMLoadFloat4x4(&m_pParent->GetWorldMatrix());
		mtxParentWorldRotWork = XMLoadFloat4x4(&m_pParent->GetWorldRotWorkMatrix());
	}
	// 拡縮を反映
	mtxScale = XMMatrixScaling(m_localscale.x, m_localscale.y, m_localscale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_localrotation.x), XMConvertToRadians(m_localrotation.y), XMConvertToRadians(m_localrotation.z));
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	mtxWorldRotWork = XMMatrixMultiply(mtxWorldRotWork, mtxRot);
	// 移動を反映
	mtxTranslate = XMMatrixTranslation(m_localposition.x, m_localposition.y, m_localposition.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	// 親のワールドマトリックスを掛け合わせる
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxParentWorld);
	mtxWorldRotWork = XMMatrixMultiply(mtxWorldRotWork, mtxParentWorldRotWork);
	// ワールドマトリックスの設定
	XMStoreFloat4x4(&m_world, mtxWorld);
	XMStoreFloat4x4(&m_worldRotWork, mtxWorldRotWork);
	// ワールド位置をセット
	m_worldposition.x = m_world._41;
	m_worldposition.y = m_world._42;
	m_worldposition.z = m_world._43;
	// ワールド回転をセット
	XMFLOAT3 WorldRot;
	if (mtxWorldRotWork.r[2].m128_f32[1] == 1.0f)
	{
		WorldRot.x = XM_PI / 2;
		WorldRot.y = 0;
		WorldRot.z = atan2f(mtxWorldRotWork.r[1].m128_f32[0], mtxWorldRotWork.r[0].m128_f32[0]);
	}
	else if (mtxWorldRotWork.r[2].m128_f32[1] == -1.0f)
	{
		WorldRot.x = -XM_PI / 2;
		WorldRot.y = 0;
		WorldRot.z = atan2f(mtxWorldRotWork.r[1].m128_f32[0], mtxWorldRotWork.r[0].m128_f32[0]);
	}
	else
	{
		WorldRot.x = asinf(-mtxWorldRotWork.r[2].m128_f32[1]);
		WorldRot.y = atan2f(mtxWorldRotWork.r[2].m128_f32[0], mtxWorldRotWork.r[2].m128_f32[2]);
		WorldRot.z = atan2f(mtxWorldRotWork.r[0].m128_f32[1], mtxWorldRotWork.r[1].m128_f32[1]);
	}
	m_worldrotation = XMFLOAT3(XMConvertToDegrees(WorldRot.x), XMConvertToDegrees(WorldRot.y), XMConvertToDegrees(WorldRot.z));
	XMFLOAT3 parscl = XMFLOAT3(1.0f, 1.0f, 1.0f);
	if (m_pParent)
		parscl = m_pParent->GetWorldScale();
	m_worldscale = XMFLOAT3(m_localscale.x * parscl.x, m_localscale.y * parscl.y, m_localscale.z * parscl.z);

	// 子オブジェクトのワールドマトリックスも更新
	if (m_pChild != nullptr)
	{
		m_pChild->UpdateMatrix();
		CObject* pObjChild = m_pChild;
		while (pObjChild->m_pBrotherNext != nullptr)
		{
			pObjChild = pObjChild->m_pBrotherNext;
			pObjChild->UpdateMatrix();
		}
	}
}

// 引数のオブジェクトを子供に持つ場合、その情報を削除する
void CObject::DeleteChild(CObject* DelObj)	
{
	if (DelObj == nullptr)return;
	CObject* pObjChild,*pBeforObjChild;
	pBeforObjChild = nullptr;
	pObjChild = m_pChild;
	while (pObjChild != nullptr)
	{
		if (pObjChild == DelObj)
		{	// 削除対象があった
			if (pBeforObjChild != nullptr)
			{	// 先頭じゃないとき
				// 自身をリストから抜く
				pBeforObjChild->m_pBrotherNext = pObjChild->m_pBrotherNext;
			}
			else
			{	// 先頭の時
				m_pChild = pObjChild->m_pBrotherNext;
			}
		}
		pBeforObjChild = pObjChild;
		pObjChild = pObjChild->m_pBrotherNext;
	}
	
}
void CObject::SetParent(CObject* obj)
{
	// 親にセット
	m_pParent = obj;
	if (obj == nullptr)return;	// 引数がnullptrだったら終わる

	// nullptrじゃなかったら、このオブジェクトを引数オブジェクトの子供として登録
	CObject* pObjChild = obj->m_pChild;
	if (pObjChild == nullptr)
	{	// まだ子オブジェクトがない
		pObjChild = this;
	}
	while (pObjChild->m_pBrotherNext != nullptr)
	{
		pObjChild = m_pBrotherNext;
	}
	pObjChild->m_pBrotherNext = this;
	
}

CObject* CObject::GetParent()
{
	return m_pParent;
}

void CObject::SetSize(XMFLOAT3 size) 
{
	if (size.x < 0.0f)size.x = 0.0f;
	if (size.y < 0.0f)size.y = 0.0f;
	if (size.z < 0.0f)size.z = 0.0f;
	m_localscale = size;
}
void CObject::SetPos(XMFLOAT3 pos) 
{ 
	m_localposition = pos; 
}
void CObject::SetRot(XMFLOAT3 rot)
{
	while (rot.x > 180.0f) { rot.x -= 360.0f; }
	while (rot.x < -180.0f) { rot.x += 360.0f; }
	while (rot.y > 180.0f) { rot.y -= 360.0f; }
	while (rot.y < -180.0f) { rot.y += 360.0f; }
	while (rot.z > 180.0f) { rot.z -= 360.0f; }
	while (rot.z < -180.0f) { rot.z += 360.0f; }
	m_localrotation = rot;
}
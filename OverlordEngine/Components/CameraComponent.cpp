#include "stdafx.h"
#include "CameraComponent.h"
#include <iostream>
CameraComponent::CameraComponent(bool enableTargeting) :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(45),
	m_Size(25.0f),
	m_PerspectiveProjection(true),
	m_enableTargeting{enableTargeting}
{
	XMStoreFloat4x4(&m_Projection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, XMMatrixIdentity());
}

void CameraComponent::UpdateRotation(const SceneContext& sceneContext, const XMVECTOR& upVec, const XMVECTOR& lookAt)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());

	m_LookAt = lookAt;
	m_UpVec = upVec;

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + m_LookAt, m_UpVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Update(const SceneContext& sceneContext)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	XMMATRIX projection{};

	if (m_PerspectiveProjection)
	{
		projection = XMMatrixPerspectiveFovLH(m_FOV, sceneContext.aspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * sceneContext.aspectRatio : sceneContext.windowWidth;
		const float viewHeight = (m_Size > 0) ? m_Size : sceneContext.windowHeight;
		projection = XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());


	if (!m_enableTargeting) {
		m_LookAt = XMLoadFloat3(&GetTransform()->GetForward());
		m_UpVec = XMLoadFloat3(&GetTransform()->GetUp());
	}

	const XMMATRIX view = XMMatrixLookAtLH(worldPosition, worldPosition + m_LookAt, m_UpVec);
	const XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::SetActive(bool active)
{
	if (m_IsActive == active) return;

	const auto pGameObject = GetGameObject();
	ASSERT_IF(!pGameObject, L"Failed to set active camera. Parent game object is null");

	if (!pGameObject) return; //help the compiler... (C6011)
	const auto pScene = pGameObject->GetScene();
	ASSERT_IF(!pScene, L"Failed to set active camera. Parent game scene is null");

	m_IsActive = active;
	pScene->SetActiveCamera(active?this:nullptr); //Switch to default camera if active==false
}

GameObject* CameraComponent::Pick(CollisionGroup ignoreGroups) const
{
	//convert mouse coordinates to NDC
	const auto& mousePos = InputManager::GetMousePosition();
	const float windowWidth = m_pScene->GetSceneContext().windowWidth;
	const float windowHeight = m_pScene->GetSceneContext().windowHeight;
	const float halfWidth = windowWidth / 2.0f;
	const float halfHeight = windowHeight / 2.0f;

	const float x = (mousePos.x - halfWidth) / halfWidth;
	const float y = (halfHeight - mousePos.y) / halfWidth;

	// calculate nearpoint and farpoint
	XMFLOAT4X4 viewProjectionInv{};
	XMStoreFloat4x4(&viewProjectionInv, XMMatrixInverse(nullptr, XMLoadFloat4x4(&GetViewProjection())));

	XMFLOAT4 nearPoint{ x, y, 0.0f, 0.0f };
	XMFLOAT4 farPoint{ x, y, 1.0f, 0.0f };

	XMVECTOR nearPointV = XMLoadFloat4(&nearPoint);
	XMVECTOR farPointV = XMLoadFloat4(&farPoint);

	XMVECTOR nearPointWorld = XMVector3TransformCoord(nearPointV, XMLoadFloat4x4(&viewProjectionInv));
	XMVECTOR farPointWorld = XMVector3TransformCoord(farPointV, XMLoadFloat4x4(&viewProjectionInv));

	PxQueryFilterData filterData{};
	filterData.data.word0 = ~UINT(ignoreGroups);

	PxRaycastBuffer hit{};
	// cast nearpointworld to pxvec3
	PxVec3 origin = PxVec3(XMVectorGetX(nearPointWorld), XMVectorGetY(nearPointWorld), XMVectorGetZ(nearPointWorld));
	PxVec3 direction = PxVec3(XMVectorGetX(farPointWorld - nearPointWorld), XMVectorGetY(farPointWorld - nearPointWorld), XMVectorGetZ(farPointWorld - nearPointWorld));
	float distance = PX_MAX_F32;

	direction.normalize();

	if (m_pScene->GetPhysxProxy()->Raycast(origin, direction, distance, hit, PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.hasBlock)
		{
			return static_cast<BaseComponent*>(hit.block.actor->userData)->GetGameObject();
		}
	}

	return nullptr;
}


XMVECTOR CameraComponent::GetRightVec()
{
	// calculate right vec (cross lookat and up)
	XMVECTOR rightVec = XMVector3Cross(XMLoadFloat3(&GetTransform()->GetForward()), XMLoadFloat3(&GetTransform()->GetUp()));
	rightVec = XMVector3Normalize(rightVec);
	return rightVec;
}

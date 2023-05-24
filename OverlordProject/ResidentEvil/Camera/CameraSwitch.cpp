#include "stdafx.h"
#include "CameraSwitch.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "Prefabs/CubePrefab.h"

CameraSwitch::CameraSwitch(const XMFLOAT3& pos, const XMFLOAT3& size, bool debug)
	: m_TargetCam0{0}, m_TargetCam1{1}
{
	InitializeTriggerBox(pos, size);


	if (!debug) return;

	AddChild(new CubePrefab());
}

void CameraSwitch::InitializeTriggerBox(const XMFLOAT3& pos, const XMFLOAT3& size)
{
	auto pDefaultMaterial = PhysXManager::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.f);

	auto triggerRb = AddComponent(new RigidBodyComponent());
	triggerRb->SetKinematic(true);

	triggerRb->AddCollider(PxBoxGeometry(size.x * .5f, size.y * .5f, size.z * .5f), *pDefaultMaterial, true);
	SetOnTriggerCallBack([this](GameObject* trigger, GameObject* other, PxTriggerAction action)
	{
		OnTrigger(trigger, other, action);
	});

	GetTransform()->Translate(pos);
}

void CameraSwitch::OnTrigger(GameObject * trigger, GameObject * other, PxTriggerAction action)
{
	if (action == PxTriggerAction::ENTER && other->GetTag() == L"Player" && trigger == this)
	{
		auto currentCam = ReCameraManager::Get().GetActiveCamera();
		ReCameraManager::Get()
			.SetActiveCamera(currentCam->GetIdx() == m_TargetCam0
				? m_TargetCam1 : m_TargetCam0);
	}
}

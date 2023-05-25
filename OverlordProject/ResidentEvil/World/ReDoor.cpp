#include "stdafx.h"
#include "ReDoor.h"
#include <iostream>
#include "Prefabs/CubePrefab.h"
#include "ResidentEvil/World/InteractableComponent.h"

ReDoor::ReDoor(const XMFLOAT3& pos, const XMFLOAT3& size)
{
	InitializeCollisionBox(pos, size);

	auto interactable = AddComponent(new InteractableComponent());
	interactable->SetInteractCallback([this]() { Interact(); });

	// debug
	AddChild(new CubePrefab(size.x, size.y, size.z));
}

void ReDoor::Initialize(const SceneContext& )
{

}

void ReDoor::InitializeCollisionBox(const XMFLOAT3& pos, const XMFLOAT3& size)
{
	auto pDefaultMaterial = PhysXManager::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.f);

	auto triggerRb = AddComponent(new RigidBodyComponent());
	triggerRb->SetKinematic(true);

	triggerRb->AddCollider(PxBoxGeometry(size.x * .5f, size.y * .5f, size.z * .5f), *pDefaultMaterial, true);
	GetTransform()->Translate(pos);
}

void ReDoor::Interact()
{
	// loading screen + load appropriate scene
	
	SceneManager::Get()->SetScene(m_sceneToLoadIdx);
}
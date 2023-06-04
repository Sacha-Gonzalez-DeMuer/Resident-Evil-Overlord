#include "stdafx.h"
#include "ReInteractableObject.h"
#include "ResidentEvil/World/InteractableComponent.h"
#include "Prefabs/CubePrefab.h"

ReInteractableObject::ReInteractableObject(const XMFLOAT3& pos, const XMFLOAT3& size)
{
	InitializeCollisionBox(pos, size);

	auto interactable = AddComponent(new InteractableComponent());
	interactable->SetInteractCallback([this]() { Interact(); });

	// debug
	//AddChild(new CubePrefab(size.x, size.y, size.z));
}

void ReInteractableObject::InitializeCollisionBox(const XMFLOAT3& pos, const XMFLOAT3& size)
{
	auto pDefaultMaterial = PhysXManager::Get()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.f);

	auto triggerRb = AddComponent(new RigidBodyComponent());
	triggerRb->SetKinematic(true);

	triggerRb->AddCollider(PxBoxGeometry(size.x * .5f, size.y * .5f, size.z * .5f), *pDefaultMaterial, true);
	GetTransform()->Translate(pos);
}

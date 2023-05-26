#include "stdafx.h"
#include "ComponentTestScene.h"
#include "Prefabs/SpherePrefab.h"

void ComponentTestScene::Initialize()
{
	auto& physX = PxGetPhysics();
	auto pBouncyMaterial = physX.createMaterial(.5f, .5f, 1.f);

	// Ground Plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	// Sphere Red (Group 0)
	const float sphereSize{ 1.0f };
	auto pSphere = new SpherePrefab(sphereSize, 10, XMFLOAT4{ Colors::Red });
	AddChild(pSphere);
	pSphere->GetTransform()->Translate(0, 30.f, 0);
	pSphere->AddComponent(new RigidBodyComponent(false));

	auto pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ sphereSize }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);

	// Sphere Green (Group 1)
	pSphere = new SpherePrefab(sphereSize, 10, XMFLOAT4{ Colors::Green });
	AddChild(pSphere);
	pSphere->GetTransform()->Translate(0, 20.f, 0);
	pSphere->AddComponent(new RigidBodyComponent(false));

	pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ sphereSize }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);

	// Sphere Blue (Group 0)
	pSphere = new SpherePrefab(sphereSize, 10, XMFLOAT4{ Colors::Blue });
	AddChild(pSphere);
	pSphere->GetTransform()->Translate(0, 40.f, 0);
	pSphere->AddComponent(new RigidBodyComponent(false));

	pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ sphereSize }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);
}

#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/ColorMaterial.h"
#include "Materials/DiffuseMaterial.h"
#include "FilePaths.h"

void ModelTestScene::Initialize()
{
	m_pChair = new GameObject();
	auto pRoughMaterial = PhysXManager::Get()->GetPhysics()->createMaterial(1.f, 1.f, 0.f);

	//// CHAIR
	// model
	auto diffuse_material{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial>() };
	diffuse_material->SetDiffuseTexture(FilePath::CHAIR_TEX);
	m_pChair->AddComponent<ModelComponent>(new ModelComponent(FilePath::CHAIR_MESH, true))->SetMaterial(diffuse_material);

	// rigidbody
	auto chair_rb = m_pChair->AddComponent<RigidBodyComponent>(new RigidBodyComponent());

	// mesh collider
	const auto chair_convex_mesh = ContentManager::Load<PxConvexMesh>(FilePath::CHAIR_CONVEX_MESH);
	chair_rb->AddCollider(PxConvexMeshGeometry{ chair_convex_mesh }, *pRoughMaterial);

	m_pChair->GetTransform()->Translate(0.f, 1.f, 0.f);


	GameSceneExt::CreatePhysXGroundPlane(*this, pRoughMaterial);


	AddChild(m_pChair);
}
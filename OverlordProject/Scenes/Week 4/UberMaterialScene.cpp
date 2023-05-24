#include "stdafx.h"
#include "UberMaterialScene.h"
#include "FilePaths.h"

void UberMaterialScene::Initialize()
{
	auto uber_sphere = new GameObject();

	m_pUberMaterial= MaterialManager::Get()->CreateMaterial<UberMaterial>();
	m_pUberMaterial->SetDiffuseTexture(FilePath::SKULL_DIFFUSE);
	m_pUberMaterial->SetNormalMapTexture(FilePath::SKULL_NORMAL);
	m_pUberMaterial->SetSpecularTexture(FilePath::SKULL_DIFFUSE);

	uber_sphere->AddComponent<ModelComponent>(new ModelComponent(FilePath::SPHERE_MESH))
		->SetMaterial(m_pUberMaterial);
	uber_sphere->GetTransform()->Scale(10);
	AddChild(uber_sphere);


	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}




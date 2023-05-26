#include "stdafx.h"
#include "SpikyScene.h"
#include "Materials/SpikyMaterial.h"
#include "FilePaths.h"
void SpikyScene::Initialize()
{
	m_pSpikyBall = new GameObject();
	auto spiky_material{ MaterialManager::Get()->CreateMaterial<SpikyMaterial>() };
	m_pSpikyBall->AddComponent<ModelComponent>(new ModelComponent(FilePath::OCTASPHERE_MESH))->SetMaterial(spiky_material);
	m_pSpikyBall->GetTransform()->Scale(15);
	AddChild(m_pSpikyBall);


	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;
}

void SpikyScene::Update()
{
	m_pSpikyBall->GetTransform()->Rotate(0, m_SceneContext.pGameTime->GetTotal() * 10, 0);
}


void SpikyScene::OnGUI()
{
}

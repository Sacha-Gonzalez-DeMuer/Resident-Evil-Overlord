#include "stdafx.h"
#include "ParticleScene.h"
#include "FilePaths.h"
#include "Materials/ColorMaterial.h"

void ParticleScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.enableOnGUI = true;

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };

	const auto pObject = AddChild(new GameObject);
	m_pEmitter = pObject->AddComponent(new ParticleEmitterComponent(L"Textures/Smoke.png", settings, 200));
	m_pEmitter->Play();

	//Teapot
	m_pTeapot = AddChild(new GameObject());
	const auto pModel = m_pTeapot->AddComponent(new ModelComponent(L"Meshes/Teapot.ovm"));

	const auto pMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial>();
	pMaterial->SetColor(XMFLOAT4{ Colors::White });
	pModel->SetMaterial(pMaterial);
	m_pTeapot->GetTransform()->Scale(.3f, .3f, .3f);
}

void ParticleScene::Update()
{
	//Rotate Teapot
	m_pTeapot->GetTransform()->Rotate(.0f, XM_PIDIV2 * m_SceneContext.pGameTime->GetTotal(), .0f, false);
}

void ParticleScene::OnGUI()
{
	ImGui::Checkbox("Auto Move", &m_AutoMove);
	m_pEmitter->DrawImGui();
}

#include "stdafx.h"
#include "SpriteTestScene.h"

void SpriteTestScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_pSprite = new GameObject();
	m_pSprite->AddComponent(new SpriteComponent(L"Textures/TestSprite.jpg", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(m_pSprite);

	m_pSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	m_pSprite->GetTransform()->Scale(1.f, 1.f, 1.f);

	m_pSecondSprite = new GameObject();
	m_pSecondSprite->AddComponent(new SpriteComponent(L"Textures/GroundBrick.jpg", { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
	AddChild(m_pSecondSprite);

	m_pSecondSprite->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, 1.f);
	m_pSecondSprite->GetTransform()->Scale(1.f, 1.f, 1.f);
}

void SpriteTestScene::Update()
{
	m_pSprite->GetTransform()->Rotate(0.f, 0.f, m_TotalRotation);

	if (m_AutoRotate)
	{
		m_TotalRotation += 30.f * m_SceneContext.pGameTime->GetElapsed();
		m_TotalRotation = fmod(m_TotalRotation, 360.f);
	}
}

void SpriteTestScene::OnGUI()
{
	const auto pSpriteComponent = m_pSprite->GetComponent<SpriteComponent>();

	ImGui::SliderFloat2("Position", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetTransform()->GetPosition()), 0, m_SceneContext.windowWidth);
	ImGui::SliderFloat2("Scale", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetTransform()->GetScale()), 0, 3);
	ImGui::SliderFloat2("Pivot", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetPivot()), 0, 1);
	ImGui::ColorEdit4("Color", ConvertUtil::ToImFloatPtr(pSpriteComponent->GetColor()));
	ImGui::Checkbox("AutoRotate", &m_AutoRotate);

	ImGui::BeginDisabled(m_AutoRotate);
	ImGui::SliderFloat("Rotation", &m_TotalRotation, 0, 360);
	ImGui::EndDisabled();


	ImGui::Separator();

	const auto pSecondSpriteComponent = m_pSecondSprite->GetComponent<SpriteComponent>();

	ImGui::SliderFloat2("Position", ConvertUtil::ToImFloatPtr(pSecondSpriteComponent->GetTransform()->GetPosition()), 0, m_SceneContext.windowWidth);
	ImGui::SliderFloat2("Scale", ConvertUtil::ToImFloatPtr(pSecondSpriteComponent->GetTransform()->GetScale()), 0, 3);
	ImGui::SliderFloat2("Pivot", ConvertUtil::ToImFloatPtr(pSecondSpriteComponent->GetPivot()), 0, 1);
	ImGui::ColorEdit4("Color", ConvertUtil::ToImFloatPtr(pSecondSpriteComponent->GetColor()));


}

#include "stdafx.h"
#include "MainMenuScene.h"

#include "ResidentEvil/ReGameManager.h"
#include "ResidentEvil/HUD/ReMenuManager.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"
#include "Components/SpriteComponent.h"
#include "ResidentEvil/ReGameManager.h"
#include "FilePaths.h"


void MainMenuScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;


	// Background
	const float centerWidth = m_SceneContext.windowWidth * .5f;
	const float centerHeight = m_SceneContext.windowHeight * .5f;

	auto pBackground = AddChild(new GameObject());
	pBackground->AddComponent(new SpriteComponent(FilePath::MAINMENU_BACKGROUND_IMG, { 0.5f, 0.5f }, { 1.f, 1.f, 1.f, 1.f }));
	pBackground->GetTransform()->Translate(centerWidth, centerHeight, 0.f);

	auto pStandby = AddChild(new GameObject());
	m_pStandBy = pStandby->AddComponent(new SpriteComponent(FilePath::MAINMENU_STANDBY_IMG, { 0.5f, 0.5f }, { 1.f, 1.f, 1.f, 1.f }));
	m_pStandBy->SetActive(false);

	auto pMenuManager = AddChild(new ReMenuManager());
	const float margin{ 35.f };
	const float offset{ 150.f };
	const XMFLOAT2 btnSize{ 128, 50 };
	SpriteFont* pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);




	// Main 
	auto pMainMenu = AddChild(new ReMenu(ReMenuType::MAIN));
	pMainMenu->GetTransform()->Scale(1.f, 1.f, 1.f);

	auto pStartBtn = new ReButton({ centerWidth, centerHeight + offset}, btnSize, pFont);
	pStartBtn->AddOnClick([this]() { StartGame(); });
	pStartBtn->SetText("START");


	auto pExitBtn = new ReButton({ centerWidth, centerHeight + offset + margin }, btnSize, pFont);
	pExitBtn->AddOnClick([this]() {
		std::cout << "Exit\n";
		});
	pExitBtn->SetText("EXIT");

	pMainMenu->AddButton(pStartBtn);
	pMainMenu->AddButton(pExitBtn);
	pMenuManager->AddMenu(pMainMenu);

	// load evil sound
	m_pFMODSys = SoundManager::Get()->GetSystem();
	const auto& evilPath = ContentManager::GetFullAssetPath(FilePath::EVIL01_AUDIO);
	auto result = m_pFMODSys->createStream(evilPath.string().c_str(), FMOD_LOOP_OFF | FMOD_2D, nullptr, &m_pEVILSound);
	if (result != FMOD_OK)
	{
		Logger::LogError(L"MainMenuScene::Initialize > Failed to load evil sound!\n" + FilePath::EVIL01_AUDIO);
	}

	const auto& standByPath = ContentManager::GetFullAssetPath(FilePath::STANDBY_AUDIO);
	result = m_pFMODSys->createStream(standByPath.string().c_str(), FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &m_pStandBySound);
	if (result != FMOD_OK)
	{
		Logger::LogError(L"MainMenuScene::Initialize > Failed to load standby sound!\n" + FilePath::STANDBY_AUDIO);
	}
}

void MainMenuScene::OnGUI()
{
	if (ImGui::CollapsingHeader("Buttons"))
	{
		static int selectedBtn{ 0 };
		ImGui::InputInt("Button Idx", &selectedBtn);
		if (selectedBtn < 0) selectedBtn = 0;
		auto pBtn = m_pButtons[selectedBtn];

		const auto& bposition = pBtn->GetTransform()->GetPosition();
		float positionArray[3] = { bposition.x, bposition.y, bposition.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = pBtn->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		pBtn->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		pBtn->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

	if (ImGui::CollapsingHeader("Menus"))
	{
		static int selectedMenu{ 0 };
		ImGui::InputInt("Menu Idx", &selectedMenu);
		auto pMenu = m_pMenus[selectedMenu];
		if (selectedMenu < 0) selectedMenu = 0;

		const auto& mposition = pMenu->GetTransform()->GetPosition();
		float positionArray[3] = { mposition.x, mposition.y, mposition.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = pMenu->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		pMenu->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		pMenu->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}
}

void MainMenuScene::StartGame()
{
	m_pStandBy->SetActive(true);
	auto pFMOD = SoundManager::Get()->GetSystem();
	pFMOD->playSound(m_pEVILSound, nullptr, false, &m_pMenuChannel);

	// fade to black
	m_pFMODSys->playSound(m_pStandBySound, nullptr, false, &m_pMenuChannel);

	ReGameManager::Get().SetSpawnPos({ 0.f, 15.f, -90.f });
	ReGameManager::Get().StartScene(ReScenes::DINING);
}

void MainMenuScene::Exit()
{
	std::cout << "leaving so soon?\n";
}

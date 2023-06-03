#include "stdafx.h"
#include "MainMenuScene.h"

#include "ResidentEvil/HUD/ReMenuManager.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"
#include "FilePaths.h"


void MainMenuScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;

	//m_pMenuManager = AddChild(new ReMenuManager());
	////const float thirdHeight{ m_SceneContext.windowHeight * .33f };
	////const float quarterHeight{ m_SceneContext.windowHeight * .25f };
	//const float btnWidth{ 5.f };
	//const float btnHeight{ 3.f };
	//const float margin{ 100.f };
	//const float centerWidth = m_SceneContext.windowWidth * .5f;
	//const float centerHeight = m_SceneContext.windowHeight * .5f;

	//SpriteFont *pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);

	//float depth{ 0 };

	//// Main 
	//auto pMainMenu = AddChild(new ReMenu(ReMenuType::MAIN));
	//pMainMenu->GetTransform()->Scale(1.f, 1.f, 1.f);

	//auto pStartBtn_null = new ReButton(pFont);
	//pStartBtn_null->GetTransform()->Scale(btnWidth, btnHeight, 0.f);
	//pStartBtn_null->SetOnClick([]() { std::cout << "Start"; });
	//pStartBtn_null->SetText("START_NULL");


	//auto pStartBtn = AddChild(new ReButton(pFont));
	//pStartBtn->GetTransform()->Scale(btnWidth, btnHeight, 0.f);
	//pStartBtn->SetOnClick([this]() { StartGame(); });
	//pStartBtn->SetText("START");

	//auto pControlsBtn = new ReButton(pFont);
	//pControlsBtn->GetTransform()->Scale(btnWidth, btnHeight, 0.f);
	//pControlsBtn->SetOnClick([&]() { m_pMenuManager->SwitchMenu(ReMenuType::CONTROLS); });
	//pControlsBtn->SetText("CONTROLS");
	//
	//auto pExitBtn = new ReButton(pFont);
	//pExitBtn->GetTransform()->Scale(btnWidth, btnHeight, 0.f);
	//pExitBtn->SetOnClick([this]() { Exit(); });
	//pExitBtn->SetText("EXIT");


	//pMainMenu->AddButton(pExitBtn);
	//pMainMenu->AddButton(pControlsBtn);
	//pMainMenu->AddButton(pStartBtn_null);
	//m_pMenuManager->AddMenu(pMainMenu);

	//pStartBtn_null->GetTransform()->Translate(centerWidth, centerHeight - margin, depth);
	////pStartBtn->GetTransform()->Translate(centerWidth, centerHeight - margin, depth);
	//pControlsBtn->GetTransform()->Translate(centerWidth, centerHeight, depth);
	//pExitBtn->GetTransform()->Translate(centerWidth, centerHeight + margin, depth);

	//m_pButtons.push_back(pStartBtn);
	//m_pButtons.push_back(pExitBtn);
	//m_pButtons.push_back(pControlsBtn);
	//m_pMenus.push_back(pMainMenu);

	////// Controls
	////auto pControlsMenu = AddChild(new ReMenu(ReMenuType::CONTROLS)); // todo add controls sprite
	////auto pBackBtn = pControlsMenu->AddChild(new ReButton(pFont));
	////pBackBtn->SetText("BACK");
	////pBackBtn->GetTransform()->Scale(btnWidth, btnHeight, 1.f);
	////pBackBtn->GetTransform()->Translate(0, 0, 0.f);
	////pBackBtn->SetOnClick([this]() {  m_pMenuManager->SwitchMenu(ReMenuType::CONTROLS); });
	////pControlsMenu->AddButton(pBackBtn);

	////m_pMenuManager->AddMenu(pMainMenu);
	////m_pMenuManager->AddMenu(pControlsMenu);

	////m_pMenuManager->DisableMenus();
	////m_pMenuManager->SwitchMenu(ReMenuType::MAIN);
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
	SceneManager::Get()->SetActiveGameScene(L"DiningHallScene");
}

void MainMenuScene::Exit()
{
	std::cout << "leaving so soon?\n";
}

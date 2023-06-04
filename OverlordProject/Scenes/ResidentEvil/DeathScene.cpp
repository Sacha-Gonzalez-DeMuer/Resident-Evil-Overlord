#include "stdafx.h"
#include "DeathScene.h"
#include "FilePaths.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"
#include "ResidentEvil/HUD/ReMenuManager.h"
#include "ResidentEvil/ReGameManager.h"

void DeathScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.showInfoOverlay = false;

	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;
	ReGameManager::Get();

	// Background
	const float centerWidth = m_SceneContext.windowWidth * .5f;
	const float centerHeight = m_SceneContext.windowHeight * .5f;
	const XMFLOAT2 btnSize{ 75, 25 };
	const float margin{ 35.f };
	const float offset{ 170.f };
	SpriteFont* pFont = ContentManager::Load<SpriteFont>(FilePath::SUB_FONT);
	auto pBackground = AddChild(new GameObject());
	m_pDeathScreen = pBackground->AddComponent(new SpriteComponent(FilePath::DEATH_IMG, { 0.5f, 0.5f }, { 1.f, 1.f, 1.f, 1.f }));
	pBackground->GetTransform()->Translate(centerWidth, centerHeight, 0.f);

	m_pMenuManager = AddChild(new ReMenuManager());
	// Main 
	auto pMainMenu = AddChild(new ReMenu(ReMenuType::MAIN));
	pMainMenu->GetTransform()->Scale(1.f, 1.f, 1.f);

	auto pToMainBtn = AddChild(new ReButton({ centerWidth - btnSize.x * .5f, centerHeight + offset }, btnSize, pFont));
	pToMainBtn->AddOnClick([this]() { ReGameManager::Get().StartScene(ReScenes::MENU); });
	pToMainBtn->SetText("MAIN MENU");

	auto pReset = AddChild(new ReButton({ centerWidth - btnSize.x * .5f, centerHeight + offset + margin }, btnSize, pFont));
	pReset->SetText("RESET");
	pReset->AddOnClick([this]() { RestartGame(); });

	auto pExitBtn = AddChild(new ReButton({ centerWidth - btnSize.x * .5f, centerHeight + offset + margin * 2 }, btnSize, pFont));
	pExitBtn->AddOnClick([this]() { ReGameManager::Get().Exit(); });
	pExitBtn->SetText("EXIT");

	pMainMenu->AddButton(pToMainBtn);
	pMainMenu->AddButton(pReset);
	pMainMenu->AddImage(m_pDeathScreen);
	m_pMenuManager->AddMenu(pMainMenu);

	// Audio
	auto pFMOD = SoundManager::Get()->GetSystem();
	const auto& deathMusicPath = ContentManager::GetFullAssetPath(FilePath::DEATH_MUSIC);
	auto result = pFMOD->createStream(deathMusicPath.string().c_str(), FMOD_LOOP_NORMAL | FMOD_2D,nullptr, &m_pDeathMusic);
	if (result != FMOD_OK)
	{
		Logger::LogError(L"DeathScene::Initialize() > Failed to create stream! Error: " + std::to_wstring(result));
		return;
	}
}

void DeathScene::Start()
{
	auto pFMOD = SoundManager::Get()->GetSystem();
	auto result = pFMOD->playSound(m_pDeathMusic, nullptr, false, &m_pDeathChannel);
	if (result != FMOD_OK)
	{
		Logger::LogError(L"DeathScene::Start() > Failed to play sound! Error: " + std::to_wstring(result));
		return;
	}
}

void DeathScene::Reset()
{}

void DeathScene::RestartGame()
{
	m_pDeathChannel->stop();
	ReGameManager::Get().StartScene(ReScenes::DINING);
}

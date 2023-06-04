#include "stdafx.h"
#include "ReGameManager.h"
#include "Scenes/ResidentEvil/MainMenuScene.h"
#include "Scenes/ResidentEvil/DiningHallScene.h"
#include "Scenes/ResidentEvil/MainHallScene.h"
#include "ResidentEvil/Camera/ReCameraManager.h"

void ReGameManager::LoadResidentEvil()
{
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MENU, { L"MainMenu", new MainMenuScene() }));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::DINING, {L"DiningHallScene", new DiningHallScene()}));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MAIN, {L"MainHallScene", new MainHallScene()}));

	ReCameraManager::Get(); // initalize manager
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MENU].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::DINING].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MAIN].pScene);
}

void ReGameManager::StartScene(ReScenes scene)
{
	m_ScenesMap[scene].pScene->Start();
}

void ReGameManager::Exit()
{
	PostQuitMessage(0);
}

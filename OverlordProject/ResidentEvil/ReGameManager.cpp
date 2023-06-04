#include "stdafx.h"
#include "ReGameManager.h"
#include "Scenes/ResidentEvil/MainMenuScene.h"
#include "Scenes/ResidentEvil/DiningHallScene.h"
#include "Scenes/ResidentEvil/MainHallScene.h"

void ReGameManager::LoadResidentEvil()
{
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MENU, { L"MainMenuScene", new MainMenuScene() }));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::DINING, {L"DiningHallScene", new DiningHallScene()}));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MAIN, {L"MainHallScene", new MainHallScene()}));

	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MENU].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::DINING].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MAIN].pScene);
}

void ReGameManager::StartScene(ReScenes scene)
{
	SceneManager::Get()->SetActiveGameScene(m_ScenesMap[scene].sceneName);
	m_ScenesMap[scene].pScene->Start();
}

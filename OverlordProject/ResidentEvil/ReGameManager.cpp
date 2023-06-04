#include "stdafx.h"
#include "ReGameManager.h"
#include "Scenes/ResidentEvil/MainMenuScene.h"
#include "Scenes/ResidentEvil/DiningHallScene.h"
#include "Scenes/ResidentEvil/MainHallScene.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "Scenes/ResidentEvil/DeathScene.h"
#include "Scenes/ResidentEvil/ReScene.h"

void ReGameManager::LoadResidentEvil()
{
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MENU, { L"MainMenu", new MainMenuScene() }));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::DINING, {L"DiningHallScene", new DiningHallScene()}));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::MAIN, {L"MainHallScene", new MainHallScene()}));
	m_ScenesMap.insert(std::pair<ReScenes, ReSceneData>(ReScenes::DEATH, {L"DeathScene", new DeathScene()}));

	ReCameraManager::Get(); // initalize manager
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MENU].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::DINING].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::MAIN].pScene);
	SceneManager::Get()->AddGameScene(m_ScenesMap[ReScenes::DEATH].pScene);
}

void ReGameManager::StartScene(ReScenes scene)
{
	if (m_CurrentScene) m_CurrentScene->Reset();
	m_CurrentScene = m_ScenesMap[scene].pScene;
	m_CurrentScene->Start();
	SceneManager::Get()->SetActiveGameScene(m_ScenesMap[scene].sceneName);
}

void ReGameManager::Exit()
{
	PostQuitMessage(0);
}

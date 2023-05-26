#include "stdafx.h"
#include "MainHallScene.h"

#include "FilePaths.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "Utils/StaticMeshFactory.h"

MainHallScene::MainHallScene()
	: GameScene(L"MainHallScene")
{
}

void MainHallScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = true;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;

	auto pMainHall = AddChild(new GameObject());

	// Add hall model and textures  
	std::wstring mainhall_folder = ContentManager::GetFullAssetPath(FilePath::FOLDER_ENV_MAINHALL);
	StaticMeshFactory::Get()->AddMtlModelComponent(*pMainHall, mainhall_folder);
	pMainHall->GetTransform()->Scale(100.f, 100.f, 100.f);

}

void MainHallScene::Update()
{
}

void MainHallScene::AddLights()
{
}

void MainHallScene::AddCameras()
{
}

void MainHallScene::AddCameraSwitches()
{
}

void MainHallScene::AddDoors()
{
}


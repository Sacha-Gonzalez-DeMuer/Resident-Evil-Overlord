#include "stdafx.h"
#include "StairsScene.h"

#include "Utils/FbxLoader.h"
#include "FilePaths.h"


StairsScene::StairsScene() 
	: GameScene(L"StairsScene")
{
}

void StairsScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;
	//const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);


	auto pStairs = AddChild(new GameObject());
	std::wstring stairs_fbx_path = ContentManager::GetFullAssetPath(FilePath::ENV_STAIRS_FBX);
	const auto& stairs_fbx_path_c = StringUtil::ConvertWStringToChar(stairs_fbx_path);
	dae::FbxLoader loader{ stairs_fbx_path_c };
	delete[] stairs_fbx_path_c;


	loader.LoadToOverlord(*pStairs, m_SceneContext, FilePath::FOLDER_ENV_STAIRS);

}

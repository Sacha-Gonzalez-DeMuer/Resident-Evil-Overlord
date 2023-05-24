#include "stdafx.h"
#include "PrototypingScene.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/DiffuseMaterial.h"
#include "FilePaths.h"
#include "Utils/StaticMeshFactory.h"
#include "Utils/FbxLoader.h"
#include <codecvt>
#include <locale>
#include <iostream>

#include "ResidentEvil/Camera/ReCameraManager.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Player/RePlayerController.h"
#include "ResidentEvil/Camera/CameraSwitch.h"
PrototypingScene::PrototypingScene(void) : GameScene(L"PrototypingScene")
{
}

PrototypingScene::~PrototypingScene(void)
{
}


char* ConvertWStringToChar(std::wstring str) /* courtesy of chatGPT */
{
	// Get the length of the converted string
	int length = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

	// Allocate memory for the converted string
	char* buffer = new char[length];

	// Convert the string
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, buffer, length, NULL, NULL);

	// Return the converted string
	return buffer;
}


void PrototypingScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	//Ground Plane
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// Player Model
	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(FilePath::SOLDIER_BODY_DIFFUSE);

	pPlayerObject = AddChild(new GameObject());
	auto pModel = pPlayerObject->AddComponent(new ModelComponent(FilePath::SOLDIER_WALKING_OVM));
	pModel->SetMaterial(pSkinnedMaterial);
	pModel->GetTransform()->Scale(0.02f, 0.02f, 0.02f);
	pModel->GetTransform()->Rotate(0.f, 180.f, 0.f);

	//Character
	ReCharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.controller.height = 17.5f;
	characterDesc.cameraOffset = XMFLOAT3(0.f, 1.5f, -5.f);
	characterDesc.maxMoveSpeed = 15.f;
	characterDesc.isNavMeshController = true;
	characterDesc.moveAccelerationTime = 0.01f;


	m_pCharacter = AddChild(new RePlayerController(characterDesc));

	m_pCharacter->GetTransform()->Translate(0.f, 10.f, -90.f); // spawn pos
	//m_pCharacter->AddChild(pPlayerObject);

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);

	m_SceneContext.useDeferredRendering = true;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;

	auto pDiningHall = AddChild(new GameObject());
	std::wstring dining_fbx_path = ContentManager::GetFullAssetPath(FilePath::ENV_DINING_FBX);
	dae::FbxLoader loader{ ConvertWStringToChar(dining_fbx_path) };
	//loader.SetModelOffset({ -5.f, -25.f, -10.f });

	loader.LoadToOverlord(*pDiningHall, m_SceneContext, FilePath::FOLDER_ENV_DINING);
	
	auto& pCamVolumeManager = ReCameraManager::Get();
	AddCameras();
	pCamVolumeManager.SetActiveCamera(UINT(0));

	AddCameraSwitches();

	//auto pCamVol0 = AddChild(new CameraVolume());


	//pDiningHall->AddComponent(new ModelComponent(FilePath::ENV_MAINHALL_OVM));
}


void PrototypingScene::Update()
{
	//auto cam = GetActiveCamera();
	//std::cout 
	//	<< m_SceneContext.pCamera->GetTransform()->GetPosition().x 
	//	<< " " << cam->GetTransform()->GetPosition().y 
	//	<< " " << cam->GetTransform()->GetPosition().z << std::endl;


	//Logger::LogDebug(L"Camera Position: " + std::to_wstring(m_SceneContext.pCamera->GetTransform()->GetPosition().x) + L" " + std::to_wstring(m_SceneContext.pCamera->GetTransform()->GetPosition().y) + L" " + std::to_wstring(m_SceneContext.pCamera->GetTransform()->GetPosition().z));

	//// make model follow player controller because simply setting it as child doesn't work??
	//auto controllerPos = m_pCharacter->GetTransform()->GetPosition();
	//controllerPos.y -= 1.f;
	//pPlayerObject->GetTransform()->Translate(controllerPos);

	//auto cam = m_SceneContext.pCamera;
	//auto transform = cam->GetTransform();
}

void PrototypingScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { .3f, .3f }, { 1.f,0.f });
}

void PrototypingScene::AddCameras()
{
	XMFLOAT3 camPos{ -8.0f, 11.0f, -21.0f };
	XMVECTOR camUp{ -0.0475053f, 0.983929f, 0.172127f };
	XMVECTOR camLook{ 0.261768f, 0.178562f, -0.948469f };
	float fov = 0.726f;

	//***DINING BOTTOM****
	//[0] - Door
	auto reCam = new ReCamera(camPos);
	auto cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	ReCameraManager::Get().AddVolume(reCam);

	//[1] - Clock
	camPos = { -3, 18, 11 };
	camLook = { 0.787276f, -0.178562f, -0.59018f };
	camUp = { 0.142874f, 0.983929f, -0.107105f };
	reCam = new ReCamera(camPos, true);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	ReCameraManager::Get().AddVolume(reCam);

	//[2] - Table
	camPos = { -9, 17, -43 };
	camLook = { 0.406008f, -0.0598116f, 0.91191f };
	camUp = { 0.0243276f, 0.99821f, 0.0546406f };
	reCam = new ReCamera(camPos, true);
	fov = 0.681f;
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	ReCameraManager::Get().AddVolume(reCam);

	//[3] - Fire
	camPos = { -23, 27, 38 };
	camLook = { 0.584543f, -0.294758f, 0.755928f };
	camUp = { 0.180309f, 0.955572f, 0.233175f };
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	ReCameraManager::Get().AddVolume(reCam);


	/*
	camPos = { 0, 0, 0 };
	camLook = { 0, 0, 0 };
	camUp = { 0, 0, 0 };
	reCam = new ReCamera(camPos, false);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	ReCameraManager::Get().AddVolume(reCam);
	*/
}

void PrototypingScene::AddCameraSwitches()
{
	XMFLOAT3 pos = { 0, 0, 0 };
	XMFLOAT3 size = { 1, 1, 1 };

	pos = { 25, 8, -43 };
	size = { 31, 38, 3 };
	auto pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(0, 1);
	m_pSwitches.push_back(pSwitch);

	pos = { 24, 9, 15 };
	size = { 28, 27, 2 };
	pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(1, 2);
	m_pSwitches.push_back(pSwitch);

	pSwitch = AddChild(new CameraSwitch({ 0, 0, 0 }, { 1, 1, 1 }, true));
	m_pSwitches.push_back(pSwitch);

	/*
	m_pSwitches.push_back(pSwitch);
	*/
}

void SaveCameraVolSettings(const XMVECTOR& lookatVec, const XMVECTOR& upVec, const float positionArray[3], float fov, const int volumeIdx)
{
	// Open a file for writing
	std::ofstream outputFile("variables.txt");

	if (outputFile.is_open())
	{
		outputFile << "[" << volumeIdx << "]" << std::endl;
		// Write the variables to the file
		outputFile << "camPos = { " << positionArray[0] << ", " << positionArray[1] << ", " << positionArray[2] << "};" << std::endl;
		outputFile << "lookatVec = { " << XMVectorGetX(lookatVec) << ", " << XMVectorGetY(lookatVec) << ", " << XMVectorGetZ(lookatVec) << "};" << std::endl;
		outputFile << "upVec = { " << XMVectorGetX(upVec) << ", " << XMVectorGetY(upVec) << ", " << XMVectorGetZ(upVec) << "};" << std::endl;
		outputFile << "fov = " << fov << ";" << std::endl;
		// Close the file
		outputFile.close();

		// Inform the user that the variables have been saved
		std::cout << "Variables saved to variables.txt" << std::endl;
	}
	else
	{
		// Error opening the file
		std::cout << "Unable to open file variables.txt for writing" << std::endl;
	}
}

bool camunlocked = false;
void PrototypingScene::OnGUI()
{
	DeferredRenderer::Get()->DrawImGui();

	if (ImGui::CollapsingHeader("Camera"))
	{
		if (!camunlocked)
		{
			auto activeVolume = ReCameraManager::Get().GetActiveCamera();
			auto volumeCam = activeVolume->GetCamera();

			auto position = volumeCam->GetTransform()->GetPosition();
			int camIdx = activeVolume->GetIdx();

			ImGui::InputInt("Cam Volume Idx", &camIdx);
			ReCameraManager::Get().SetActiveCamera(camIdx);

			ImGui::SliderFloat("Pitch", &activeVolume->GetPitch(), -360.0f, 360.0f);
			ImGui::SliderFloat("Yaw", &activeVolume->GetYaw(), -360.0f, 360.0f);
			ImGui::SliderFloat("Roll", &activeVolume->GetRoll(), -360.0f, 360.0f);

			float positionArray[3] = { position.x, position.y, position.z };
			ImGui::DragFloat3("Position", positionArray);
			activeVolume->GetCamera()->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

			ImGui::SliderFloat("FOV", &activeVolume->GetFOV(), 0.0f, PxPi / 2);

			if (ImGui::Button("Save Variables"))
			{
				SaveCameraVolSettings(volumeCam->GetLookAt(), volumeCam->GetUpVec(), positionArray, activeVolume->GetFOV(), activeVolume->GetIdx());
			}

			if (ImGui::Button("Unlock Camera"))
			{
				camunlocked = true;
				SetActiveCamera(nullptr);
			}
		}
	}

	//********* CAM SWITCH CONTROL
	if (ImGui::CollapsingHeader("Switches"))
	{
		static int selectedSwitch;
		ImGui::InputInt("Switch Idx", &selectedSwitch);
		auto pSwitch = m_pSwitches[selectedSwitch];

		auto position = pSwitch->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);
		pSwitch->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

		auto size = pSwitch->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);
		pSwitch->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

}
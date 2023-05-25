#include "stdafx.h"
#include "DiningHallScene.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/DiffuseMaterial.h"
#include "FilePaths.h"
#include "Utils/StaticMeshFactory.h"
#include "Utils/FbxLoader.h"
#include "Utils/Utils.h"
#include <codecvt>
#include <locale>
#include <iostream>

#include "ResidentEvil/Camera/ReCameraManager.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/CameraSwitch.h"
#include "ResidentEvil/Player/RePlayerController.h"
#include "ResidentEvil/World/ReDoor.h"

DiningHallScene::DiningHallScene(void) : GameScene(L"DiningHallScene")
{
}

DiningHallScene::~DiningHallScene(void)
{
}

ModelComponent* g_pModel;
void DiningHallScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = true;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = false;

	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	// navmesh
	auto nav_go = AddChild(new GameObject());
	auto pNavTriangleMesh = ContentManager::Load<PxTriangleMesh>(FilePath::ENV_DINING_NAVMESH);
	auto pNavRb = nav_go->AddComponent(new RigidBodyComponent(true));
	pNavRb->SetCollisionGroup(CollisionGroup::Group0);
	pNavRb->AddCollider(PxTriangleMeshGeometry(pNavTriangleMesh), *pDefaultMaterial);

	//Character
	ReCharacterDesc characterDesc{ pDefaultMaterial };
	characterDesc.actionId_MoveForward = CharacterMoveForward;
	characterDesc.actionId_MoveBackward = CharacterMoveBackward;
	characterDesc.actionId_MoveLeft = CharacterMoveLeft;
	characterDesc.actionId_MoveRight = CharacterMoveRight;
	characterDesc.actionId_Jump = CharacterJump;
	characterDesc.actionId_Interact = Interact;
	characterDesc.actionId_Sprint = CharacterSprint;

	characterDesc.controller.height = 17.5f;
	characterDesc.maxMoveSpeed = 15.f;
	characterDesc.moveAccelerationTime = 0.01f;

	m_pCharacter = AddChild(new RePlayerController(characterDesc));
	m_pCharacter->GetTransform()->Translate(0.f, 15.f, -90.f); // spawn pos

	//Input
	auto inputAction = InputAction(CharacterMoveLeft, InputState::down, 'A');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveRight, InputState::down, 'D');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveForward, InputState::down, 'W');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterMoveBackward, InputState::down, 'S');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Interact, InputState::down, 'E');
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterSprint, InputState::down, VK_SHIFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(CharacterJump, InputState::pressed, VK_SPACE, -1, XINPUT_GAMEPAD_A);
	m_SceneContext.pInput->AddInputAction(inputAction);


	auto pDiningHall = AddChild(new GameObject());
	std::wstring dining_fbx_path = ContentManager::GetFullAssetPath(FilePath::ENV_DINING_FBX);
	dae::FbxLoader loader{ StringUtil::ConvertWStringToChar(dining_fbx_path) };
	loader.LoadToOverlord(*pDiningHall, m_SceneContext, FilePath::FOLDER_ENV_DINING);
	
	auto& pCamVolumeManager = ReCameraManager::Get();
	AddCameras();

	AddCameraSwitches();
	AddDoors();
	pCamVolumeManager.SetActiveCamera(UINT(0));

	//pDiningHall->AddComponent(new ModelComponent(FilePath::ENV_MAINHALL_OVM));


}

void DiningHallScene::Update()
{

}

void DiningHallScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { .3f, .3f }, { 1.f,0.f });
}

void DiningHallScene::AddCameras()
{
	XMFLOAT3 camPos{ -8.0f, 11.0f, -21.0f };
	XMVECTOR camUp{ -0.0475053f, 0.983929f, 0.172127f };
	XMVECTOR camLook{ 0.261768f, 0.178562f, -0.948469f };
	float fov = 0.600f;

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

void DiningHallScene::AddCameraSwitches()
{
	XMFLOAT3 pos = { 0, 0, 0 };
	XMFLOAT3 size = { 1, 1, 1 };

	pos = { 25, 8, -43 };
	size = { 31, 38, 1 };
	auto pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(0, 1);
	m_pSwitches.push_back(pSwitch);

	pos = { 24, 9, 15 };
	size = { 28, 27, 1 };
	pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(1, 2);
	m_pSwitches.push_back(pSwitch);

	//pSwitch = AddChild(new CameraSwitch({ 0, 0, 0 }, { 1, 1, 1 }, true));
	//m_pSwitches.push_back(pSwitch);

	/*
	m_pSwitches.push_back(pSwitch);
	*/
}

void DiningHallScene::AddDoors()
{
	XMFLOAT3 pos{ 0.f, 17.0f, -103.0f };
	XMFLOAT3 size{ 33.0f, 30.0f, 15.0f };

	auto pDoor = AddChild(new ReDoor(pos, size));
	pDoor->SetSceneToLoad(1);
	m_pDoors.emplace_back(pDoor);
}

bool camunlocked = false;
void DiningHallScene::OnGUI()
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


	// im gui control for g_pModel position and scale
	if (ImGui::CollapsingHeader("Model"))
	{
		if (!g_pModel) return;
		auto position = g_pModel->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);
		g_pModel->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		float size = g_pModel->GetTransform()->GetScale().x;
		ImGui::DragFloat("Size", &size, .001f, 0, 1);
		g_pModel->GetTransform()->Scale(size, size, size);
	}

	if (ImGui::CollapsingHeader("Doors"))
	{
		static int selectedDoor;
		ImGui::InputInt("Door Idx", &selectedDoor);
		auto pDoor = m_pDoors[selectedDoor];

		auto position = pDoor->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);

		auto size = pDoor->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		pDoor->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		pDoor->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}
}
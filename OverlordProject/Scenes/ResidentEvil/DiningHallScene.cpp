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
#include "ResidentEvil/World/ReClock.h"
#include "ResidentEvil/HUD/SubtitleManager.h"
#include "ResidentEvil/ReData.h"
#include "ResidentEvil/NPC/ReZombie.h"
#include "ResidentEvil/Items/ReGun.h"
#include "ResidentEvil/HUD/SubtitleController.h"
#include "ResidentEvil/World/ThunderController.h"
#include "Components/ParticleEmitterComponent.h"

DiningHallScene::DiningHallScene(void) : GameScene(L"DiningHallScene")
{
}

DiningHallScene::~DiningHallScene(void)
{
}

void DiningHallScene::Initialize()
{
	m_SceneContext.settings.enableOnGUI = true;f
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;
	m_SceneContext.pLights->SetDirectionalLight({ 0, 56, 0 }, { 4, -2.43f, .040f });

	auto pSubController = AddChild(new SubtitleController());
	SubtitleManager::Get().SetController(pSubController);

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
	characterDesc.actionId_Aim = CharacterAim;
	characterDesc.actionId_Attack = CharacterAttack;
	characterDesc.controller.height = 17.5f;
	characterDesc.rotationSpeed = 120.f;
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

	inputAction = InputAction(CharacterAim, InputState::down, VK_RBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);




	auto pDiningHall = AddChild(new GameObject());
	std::wstring dining_fbx_path = ContentManager::GetFullAssetPath(FilePath::ENV_DINING_FBX);
	const auto& dining_fbx_path_c = StringUtil::ConvertWStringToChar(dining_fbx_path);
	dae::FbxLoader loader{ dining_fbx_path_c };
	delete[] dining_fbx_path_c;
	
	loader.LoadToOverlord(*pDiningHall, m_SceneContext, FilePath::FOLDER_ENV_DINING);
	
	auto& pCamVolumeManager = ReCameraManager::Get();
	AddCameras();
	AddCameraSwitches();
	AddDoors();
	pCamVolumeManager.SetActiveCamera(UINT(0));

	// Clock
	m_pClock = AddChild(new ReClock({37,14,-28}, {15,30,10}));
	Subtitle clockSub;
	clockSub.font = ContentManager::Load<SpriteFont>(FilePath::SUBTITLE_FONT);
	clockSub.text = "blablabla look at game to see \nwhat the text is";
	m_pClock->SetSubtitle(clockSub);

	// Zombie
	ReCharacterDesc zombieDesc{ pDefaultMaterial };
	zombieDesc.controller.height = 17.5f;
	zombieDesc.rotationSpeed = 60.f;
	zombieDesc.maxMoveSpeed = 10.f;
	zombieDesc.moveAccelerationTime = 0.3f;
	zombieDesc.attackDistance = 20.f;

	m_pZombie = AddChild(new ReZombie(zombieDesc));
	m_pZombie->GetTransform()->Translate(26.f, 10.f, 63.f); // spawn pos
	m_pZombie->SetTarget(m_pCharacter);
	
	//m_pGun = AddChild(new ReGun({ 8,10,-63 }, { -15,29,8 }));
	//m_pGun->SetDestInventory(m_pCharacter->GetInventory());
	ParticleEmitterSettings particleSettings{};
	particleSettings.maxSize = 5.f;
	particleSettings.maxEnergy = 3.f;
	particleSettings.minEnergy = 1.f;
	particleSettings.maxScale = 5.5f;
	particleSettings.maxEmitterRadius = 0.1f;
	particleSettings.minEmitterRadius = 0.05f;
	particleSettings.velocity = XMFLOAT3(0.f, 10.f, 0.f);



	AddChild(new ThunderController());

}

void DiningHallScene::Update()
{
}

void DiningHallScene::Draw()
{
}

void DiningHallScene::PostDraw()
{
	//Draw ShadowMap (Debug Visualization)
		//ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { .3f, .3f }, { 1.f,0.f });
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
			const auto& activeVolume = ReCameraManager::Get().GetActiveCamera();
			const auto& volumeCam = activeVolume->GetCamera();

			const auto& position = volumeCam->GetTransform()->GetPosition();
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

		const auto& position = pSwitch->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);
		pSwitch->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

		const auto& size = pSwitch->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);
		pSwitch->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}


	// im gui control for g_pModel position and scale
	//if (ImGui::CollapsingHeader("Model"))
	//{
	//	if (!g_pModel) return;
	//	auto position = g_pModel->GetTransform()->GetPosition();
	//	float positionArray[3] = { position.x, position.y, position.z };
	//	ImGui::DragFloat3("Position", positionArray);
	//	g_pModel->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
	//	float size = g_pModel->GetTransform()->GetScale().x;
	//	ImGui::DragFloat("Size", &size, .001f, 0, 1);
	//	g_pModel->GetTransform()->Scale(size, size, size);
	//}

	if (ImGui::CollapsingHeader("Doors"))
	{
		static int selectedDoor;
		ImGui::InputInt("Door Idx", &selectedDoor);
		auto pDoor = m_pDoors[selectedDoor];

		const auto& position = pDoor->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = pDoor->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		pDoor->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		pDoor->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

	//if (ImGui::CollapsingHeader("DirectionalLight"))
	//{
	//	// get light pos
	//	const auto& pLight = m_SceneContext.pLights->GetDirectionalLight();
	//	const auto& pos =  pLight.position;
	//	const auto& dir = pLight.direction;

	//	static float positionArray[3] = { pos.x, pos.y, pos.z };
	//	static float directionArray[3] = { dir.x, dir.y, dir.z };

	//	ImGui::DragFloat3("Position", positionArray, .05f, 0, 100);
	//	ImGui::DragFloat3("Direction", directionArray, .01f, -4, 4);
	//	ImGui::Checkbox("Enable", &m_SceneContext.pLights->GetDirectionalLight().isEnabled);
	//	ImGui::InputFloat("Intensity", &m_SceneContext.pLights->GetDirectionalLight().intensity, .1f, 1.f);

	//	m_SceneContext.pLights->SetDirectionalLight(
	//		{ positionArray[0], positionArray[1], positionArray[2] },
	//		{ directionArray[0], directionArray[1], directionArray[2] }
	//	);
	//}

	if (ImGui::CollapsingHeader("Clock"))
	{
		const auto& pos = m_pClock->GetTransform()->GetPosition();
		float positionArray[3] = { pos.x, pos.y, pos.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = m_pClock->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		m_pClock->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		m_pClock->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

	if (ImGui::CollapsingHeader("Gun"))
	{
		const auto& pos = m_pGun->GetTransform()->GetPosition();
		float positionArray[3] = { pos.x, pos.y, pos.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = m_pGun->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		m_pGun->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		m_pGun->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

	if (ImGui::CollapsingHeader("Zombie"))
	{
		const auto& pos = m_pZombie->GetTransform()->GetPosition();
		float positionArray[3] = { pos.x, pos.y, pos.z };
		ImGui::DragFloat3("Position", positionArray);

		const auto& size = m_pZombie->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);

		m_pZombie->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
		m_pZombie->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}

	if (ImGui::CollapsingHeader("Particle"))
	{
		const auto& pos = m_pCharacter->GetEmitter()->GetTransform()->GetPosition();
		float positionArray[3] = { pos.x, pos.y, pos.z };
		ImGui::DragFloat3("Position", positionArray);
		m_pCharacter->GetEmitter()->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
	}
}
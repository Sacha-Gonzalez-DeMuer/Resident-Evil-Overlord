#include "stdafx.h"
#include "MainHallScene.h"

#include "FilePaths.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "Utils/StaticMeshFactory.h"
#include "ResidentEvil/ReData.h"

#include "Materials/Deferred/BasicMaterial_Deferred_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"


#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/CameraSwitch.h"
#include "ResidentEvil/Player/RePlayerController.h"
#include "ResidentEvil/World/ReDoor.h"
#include "ResidentEvil/HUD/SubtitleManager.h"
#include "ResidentEvil/World/ThunderController.h"


#include "Materials/Post/PostGrain.h"
#include "Materials/Post/PostBloom.h"

MainHallScene::MainHallScene()
	: GameScene(L"MainHallScene")
{
}

void MainHallScene::Initialize()
{
	m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	AddChild(new ThunderController());
	AddLights();
	AddDoors();
	AddCameras();
	AddCameraSwitches();
	AddInput();
	AddNavCollider(*pDefaultMaterial);
	AddPlayer(pDefaultMaterial);

	LoadWorld();
}

void MainHallScene::Update()
{
}

void MainHallScene::PostDraw()
{
	if(m_DrawShadowMap)
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV();
}

void MainHallScene::OnGUI()
{
	DeferredRenderer::Get()->DrawImGui();
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);

	if (ImGui::CollapsingHeader("Camera"))
	{
		static bool camunlocked = false;
		if (!camunlocked)
		{
			const auto& activeVolume = ReCameraManager::Get().GetActiveCamera();
			const auto& volumeCam = activeVolume->GetCamera();

			const auto& position = volumeCam->GetTransform()->GetPosition();
			int camIdx = activeVolume->GetIdx();

			ImGui::InputInt("Cam Volume Idx", &camIdx);

			ImGui::SliderFloat("Pitch", &activeVolume->GetPitch(), -360.0f, 360.0f);
			ImGui::SliderFloat("Yaw", &activeVolume->GetYaw(), -360.0f, 360.0f);
			ImGui::SliderFloat("Roll", &activeVolume->GetRoll(), -360.0f, 360.0f);

			float positionArray[3] = { position.x, position.y, position.z };
			ImGui::DragFloat3("Position", positionArray);
			activeVolume->GetCamera()->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

			ImGui::SliderFloat("FOV", &activeVolume->GetFOV(), 0.0f, PxPi / 2);

			ReCameraManager::Get().SetActiveCamera(camIdx);

			if (ImGui::Button("Unlock Camera"))
			{
				camunlocked = true;
				SetActiveCamera(nullptr);
			}
		}
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		static int selectedLight{ 0 };

		ImGui::InputInt("Selected Light", &selectedLight);
		auto& light = m_SceneContext.pLights->GetLights()[selectedLight];

		float color[4] = { light.color.x, light.color.y, light.color.z, light.color.w };
		ImGui::DragFloat4("Color", color, 0.02f);
		light.color = { color[0], color[1], color[2], color[3] };

		ImGui::SliderFloat("Intensity", &light.intensity, 0, 1);
		ImGui::SliderFloat("Range", &light.range, 0.f, 100.f);

		float position[3] = { light.position.x, light.position.y, light.position.z };
		ImGui::DragFloat3("Position", position, .2f);
		light.position = { position[0], position[1], position[2], 1.0f };

		ImGui::Checkbox("Enabled", &light.isEnabled);
	}

	//********* CAM SWITCH CONTROL
	if (ImGui::CollapsingHeader("Switches"))
	{
		static int selectedSwitch;
		ImGui::InputInt("Switch Idx", &selectedSwitch);
		auto pSwitch = m_pSwitches[selectedSwitch];

		const auto& position = pSwitch->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray, .2f);
		pSwitch->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

		const auto& size = pSwitch->GetTransform()->GetScale();
		float sizeArray[3] = { size.x, size.y, size.z };
		ImGui::DragFloat3("Size", sizeArray);
		pSwitch->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
	}


	if (ImGui::CollapsingHeader("DirectionalLight"))
	{
		// get light pos
		const auto& pLight = m_SceneContext.pLights->GetDirectionalLight();
		const auto& pos =  pLight.position;
		const auto& dir = pLight.direction;

		static float positionArray[3] = { pos.x, pos.y, pos.z };
		static float directionArray[3] = { dir.x, dir.y, dir.z };

		ImGui::DragFloat3("Position", positionArray, .05f, -1000, 1000);
		ImGui::DragFloat3("Direction", directionArray, .01f, -180, 180);
		ImGui::Checkbox("Enable", &m_SceneContext.pLights->GetDirectionalLight().isEnabled);
		ImGui::InputFloat("Intensity", &m_SceneContext.pLights->GetDirectionalLight().intensity, .1f, 1.f);

		m_SceneContext.pLights->SetDirectionalLight(
			{ positionArray[0], positionArray[1], positionArray[2] },
			{ directionArray[0], directionArray[1], directionArray[2] }
		);
	}

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

	// imgui to move mainhall
	if (ImGui::CollapsingHeader("MainHall"))
	{
		if (!m_pMainHall) return;
		const auto& position = m_pMainHall->GetTransform()->GetPosition();
		float positionArray[3] = { position.x, position.y, position.z };
		ImGui::DragFloat3("Position", positionArray);
		m_pMainHall->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);
	}

}
void MainHallScene::LoadWorld()
{
	m_pMainHall = AddChild(new GameObject());
	std::wstring mainhall_folder = ContentManager::GetFullAssetPath(FilePath::FOLDER_ENV_MAINHALL);
	StaticMeshFactory::Get()->AddMtlModelComponent<BasicMaterial_Deferred_Shadow>(*m_pMainHall, mainhall_folder);
	m_pMainHall->GetTransform()->Scale(100.f, 100.f, 100.f);
	m_pMainHall->GetTransform()->Translate(-6.f, -25.f, -24.f);
}

void MainHallScene::AddLights()
{
	m_SceneContext.pLights->SetDirectionalLight({ 0, 56.6f, 62.4f }, { -.187f, -0.017f, -.993f });

	Light light{};

	// Main hall light
	light.type = LightType::Point;
	light.position = { 1.2f, 86.f, 14.8f, 1 };
	light.color = { 1, 1, 1, 1 };
	light.intensity = .5f;
	light.range = 100.f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);


	// Main hall light
	light.type = LightType::Point;
	light.position = { 0.2f, 0.f, 0, 1 };
	light.color = { 1, 1, 1, 1 };
	light.intensity = .0f;
	light.range = 10.f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);
}

void MainHallScene::AddCameras()
{
	//auto& pCamManager = ReCameraManager::Get();
	//
	//XMFLOAT3 camPos{ -36.f, 55.0f, 70.0f };
	////XMVECTOR camUp{ -0.0475053f, 0.983929f, 0.172127f };
	////XMVECTOR camLook{ 0.261768f, 0.178562f, -0.948469f };
	//float fov = 0.600f;
	//
	//auto reCam = new ReCamera(camPos);
	//reCam->GetLightOrientation() = { -.0f, -0.00f, -.0f };
	//auto cam = reCam->GetCamera();
	//cam->SetFieldOfView(fov);
	////cam->UpdateRotation(m_SceneContext, camUp, camLook);
	//AddChild(reCam);
	//pCamManager.AddVolume(reCam);
	//pCamManager.SetActiveCamera(UINT(0));

}

void MainHallScene::AddCameraSwitches()
{
	/*
m_pSwitches.push_back(pSwitch);
*/
}

void MainHallScene::AddDoors()
{
	XMFLOAT3 pos{ 0.f, 17.0f, -103.0f };
	XMFLOAT3 size{ 33.0f, 30.0f, 15.0f };

	auto pDoor = AddChild(new ReDoor(pos, size));
	pDoor->SetSceneToLoad(1);
	m_pDoors.emplace_back(pDoor);
}

void MainHallScene::AddPostProcessing()
{
	auto pMatManager = MaterialManager::Get();

	auto bloom = pMatManager->CreateMaterial<PostBloom>();
	AddPostProcessingEffect(bloom);

	auto grain = pMatManager->CreateMaterial<PostGrain>();
	AddPostProcessingEffect(grain);
}

void MainHallScene::AddInput()
{

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

}

void MainHallScene::AddNavCollider(const PxMaterial& material)
{
	auto nav_go = AddChild(new GameObject());
	auto pNavTriangleMesh = ContentManager::Load<PxTriangleMesh>(FilePath::ENV_MAIN_NAVMESH);
	auto pNavRb = nav_go->AddComponent(new RigidBodyComponent(true));
	pNavRb->SetCollisionGroup(CollisionGroup::Group0);
	pNavRb->AddCollider(PxTriangleMeshGeometry(pNavTriangleMesh), material);
}

void MainHallScene::AddPlayer(PxMaterial* material)
{
	//Character
	ReCharacterDesc characterDesc{ material };
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
	m_pCharacter->GetTransform()->Translate(0.f, characterDesc.controller.height/2.f, 0.f);
}


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
#include "Prefabs/CubePrefab.h"

#include "Materials/Post/PostGrain.h"
#include "Materials/Post/PostBloom.h"

	static int m_SelectedLight;
MainHallScene::MainHallScene()
	: GameScene(L"MainHallScene")
{
}

void MainHallScene::Initialize()
{
	m_SceneContext.settings.drawPhysXDebug = true;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.useDeferredRendering = true;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);

	m_pDebugCube = AddChild(new CubePrefab());

	//AddChild(new ThunderController());
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
	if (m_pDebugCube)
	{
		// selected light pos
		const auto& lightPos = m_SceneContext.pLights->GetLights()[m_SelectedLight].position;
		m_pDebugCube->GetTransform()->Translate(lightPos.x, lightPos.y, lightPos.z);
	}
}

void MainHallScene::PostDraw()
{
	if (m_DrawShadowMap)
		ShadowMapRenderer::Get()->Debug_DrawDepthSRV();
}

// function that saves variables to a text file
void gSaveReCamVariables(const ReCamera& cam)
{
	std::ofstream myfile;
	myfile.open("variables.txt");
	// print lookat
	XMFLOAT3 lookat;
	XMStoreFloat3(&lookat, cam.GetLookAt());
	myfile << "LookAt: " << lookat.x << " " << lookat.y << " " << lookat.z << "\n";

	// print right
	XMFLOAT3 right;
	XMStoreFloat3(&right, cam.GetRight());
	myfile << "Right: " << right.x << " " << right.y << " " << right.z << "\n";

	//print up
	XMFLOAT3 up;
	XMStoreFloat3(&up, cam.GetUp());
	myfile << "Up: " << up.x << " " << up.y << " " << up.z << "\n";

	// print position
	XMFLOAT3 position{ cam.GetTransform()->GetPosition() };
	myfile << "Position: " << position.x << " " << position.y << " " << position.z << "\n";

	myfile.close();
}

void MainHallScene::OnGUI()
{
	DeferredRenderer::Get()->DrawImGui();
	ImGui::Checkbox("Draw ShadowMap", &m_DrawShadowMap);

	if (ImGui::CollapsingHeader("Camera"))
	{
		static bool camunlocked = false;
		static bool useCam;
		if (ImGui::Button("Unlock Camera"))
		{
			camunlocked = true;
			SetActiveCamera(nullptr);
		}
		ImGui::Checkbox("Use Camera", &useCam);
		if (!camunlocked && useCam)
		{

			const auto& activeVolume = ReCameraManager::Get().GetActiveCamera();
			const auto& volumeCam = activeVolume->GetCamera();

			const auto& position = volumeCam->GetTransform()->GetPosition();
			int camIdx = activeVolume->GetIdx();
			if (ImGui::Button("SaveVariables"))
				gSaveReCamVariables(*activeVolume);

			ImGui::InputInt("Cam Volume Idx", &camIdx);
			ImGui::SliderFloat("Pitch", &activeVolume->GetPitch(), -360.0f, 360.0f);
			ImGui::SliderFloat("Yaw", &activeVolume->GetYaw(), -360.0f, 360.0f);
			ImGui::SliderFloat("Roll", &activeVolume->GetRoll(), -360.0f, 360.0f);

			float positionArray[3] = { position.x, position.y, position.z };
			ImGui::DragFloat3("Position", positionArray);
			activeVolume->GetCamera()->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

			ImGui::SliderFloat("FOV", &activeVolume->GetFOV(), 0.0f, PxPi / 2);
			

			const auto& lightOrientation = activeVolume->GetLightOrientation();
			float lightOrientationArray[3] = { lightOrientation.x, lightOrientation.y, lightOrientation.z };
			ImGui::DragFloat3("LightOrientation", lightOrientationArray, .1f);
			activeVolume->GetLightOrientation() = { lightOrientationArray[0], lightOrientationArray[1], lightOrientationArray[2], 1.0f };

			const auto& lightPosition = activeVolume->GetLightPosition();
			float lightPositionArray[3] = { lightPosition.x, lightPosition.y, lightPosition.z };
			ImGui::DragFloat3("LightPosition", lightPositionArray, .1f);
			activeVolume->GetLightPosition() = { lightPositionArray[0], lightPositionArray[1], lightPositionArray[2], 1.0f };

		}
	}

	if (ImGui::CollapsingHeader("Lights"))
	{
		static bool useLight;
		ImGui::Checkbox("Use Camera", &useLight);

		if (useLight)
		{
			ImGui::InputInt("Selected Light", &m_SelectedLight);
			auto& light = m_SceneContext.pLights->GetLights()[m_SelectedLight];

			float lightColor[4] = { light.color.x, light.color.y, light.color.z, light.color.w };
			ImGui::DragFloat4("Color", lightColor, 0.02f);
			light.color = { lightColor[0], lightColor[1], lightColor[2], lightColor[3] };

			ImGui::SliderFloat("Intensity", &light.intensity, 0, 1);
			ImGui::SliderFloat("Range", &light.range, 0.f, 100.f);

			float lposition[3] = { light.position.x, light.position.y, light.position.z };
			ImGui::DragFloat3("Position", lposition, .2f);
			light.position = { lposition[0], lposition[1], lposition[2], 1.0f };

			ImGui::Checkbox("Enabled", &light.isEnabled);
		}
	}

	//********* CAM SWITCH CONTROL
	if (ImGui::CollapsingHeader("Switches"))
	{
		static int selectedSwitch;
		static bool useSwitch;
		ImGui::Checkbox("Use Switch", &useSwitch);

		if (useSwitch)
		{
			ImGui::InputInt("Switch Idx", &selectedSwitch);
			auto pSwitch = m_pSwitches[selectedSwitch];

			const auto& switchPosition = pSwitch->GetTransform()->GetPosition();
			float positionArray[3] = { switchPosition.x, switchPosition.y, switchPosition.z };
			ImGui::DragFloat3("Position", positionArray, .2f);
			pSwitch->GetTransform()->Translate(positionArray[0], positionArray[1], positionArray[2]);

			const auto& switchSize = pSwitch->GetTransform()->GetScale();
			float switchSizeArray[3] = { switchSize.x, switchSize.y, switchSize.z };
			ImGui::DragFloat3("Size", switchSizeArray);
			pSwitch->GetTransform()->Scale(switchSizeArray[0], switchSizeArray[1], switchSizeArray[2]);
		}
	}


	if (ImGui::CollapsingHeader("DirectionalLight"))
	{
		static bool useLight;
		ImGui::Checkbox("Use DirectionalLight", &useLight);

		if (useLight)
		{
			// get light pos
			const auto& pLight = m_SceneContext.pLights->GetDirectionalLight();
			const auto& pos = pLight.position;
			const auto& dir = pLight.direction;

			static float dirPositionArray[3] = { pos.x, pos.y, pos.z };
			static float dirDirectionArray[3] = { dir.x, dir.y, dir.z };

			ImGui::DragFloat3("Position", dirPositionArray, .05f, -1000, 1000);
			ImGui::DragFloat3("Direction", dirDirectionArray, .01f, -180, 180);
			ImGui::Checkbox("Enable", &m_SceneContext.pLights->GetDirectionalLight().isEnabled);
			ImGui::InputFloat("Intensity", &m_SceneContext.pLights->GetDirectionalLight().intensity, .1f, 1.f);

			m_SceneContext.pLights->SetDirectionalLight(
				{ dirPositionArray[0], dirPositionArray[1], dirPositionArray[2] },
				{ dirDirectionArray[0], dirDirectionArray[1], dirDirectionArray[2] }
			);
		}
	}

	if (ImGui::CollapsingHeader("Doors"))
	{
		static int selectedDoor;
		static bool useDoor;
		ImGui::Checkbox("Use Door", &useDoor);

		if (useDoor)
		{
			ImGui::InputInt("Door Idx", &selectedDoor);
			auto pDoor = m_pDoors[selectedDoor];

			const auto& doorPosition = pDoor->GetTransform()->GetPosition();
			float doorPositionArray[3] = { doorPosition.x, doorPosition.y, doorPosition.z };
			ImGui::DragFloat3("Position", doorPositionArray);

			const auto& size = pDoor->GetTransform()->GetScale();
			float sizeArray[3] = { size.x, size.y, size.z };
			ImGui::DragFloat3("Size", sizeArray);

			pDoor->GetTransform()->Translate(doorPositionArray[0], doorPositionArray[1], doorPositionArray[2]);
			pDoor->GetTransform()->Scale(sizeArray[0], sizeArray[1], sizeArray[2]);
		}
	}

	//// imgui to move mainhall
	//if (ImGui::CollapsingHeader("MainHall"))
	//{
	//	if (!m_pMainHall) return;
	//	const auto& hallPosition = m_pMainHall->GetTransform()->GetPosition();
	//	float hallPositionArray[3] = { hallPosition.x, hallPosition.y, hallPosition.z };
	//	ImGui::DragFloat3("Position", hallPositionArray);
	//	m_pMainHall->GetTransform()->Translate(hallPositionArray[0], hallPositionArray[1], hallPositionArray[2]);

	//	const auto& hallSize = m_pMainHall->GetTransform()->GetScale();
	//	float fhallSize =  hallSize.x;
	//	ImGui::DragFloat("Size", &fhallSize);
	//	m_pMainHall->GetTransform()->Scale(fhallSize, fhallSize, fhallSize);
	//}

	//// imgui to transform navcollider
	//if (ImGui::CollapsingHeader("NavCollider"))
	//{
	//	if (!m_pNavCollider) return;
	//	const auto& navPosition = m_pNavCollider->GetTransform()->GetPosition();
	//	float navPositionArray[3] = { navPosition.x, navPosition.y, navPosition.z };
	//	ImGui::DragFloat3("Position", navPositionArray);
	//	m_pNavCollider->GetTransform()->Translate(navPositionArray[0], navPositionArray[1], navPositionArray[2]);

	//	const auto& navSize = m_pNavCollider->GetTransform()->GetScale();
	//	float navSizeArray[3] = { navSize.x, navSize.y, navSize.z };
	//	ImGui::DragFloat3("Size", navSizeArray);
	//	m_pNavCollider->GetTransform()->Scale(navSizeArray[0], navSizeArray[1], navSizeArray[2]);
	//}

}
void MainHallScene::LoadWorld()
{
	m_pMainHall = AddChild(new GameObject());
	std::wstring mainhall_folder = ContentManager::GetFullAssetPath(FilePath::FOLDER_ENV_MAINHALL);
	if (m_SceneContext.useDeferredRendering)
		StaticMeshFactory::Get()->AddMtlModelComponent<BasicMaterial_Deferred_Shadow>(*m_pMainHall, mainhall_folder);
	else
		StaticMeshFactory::Get()->AddMtlModelComponent<DiffuseMaterial_Shadow>(*m_pMainHall, mainhall_folder);
	m_pMainHall->GetTransform()->Scale(100.f, 100.f, 100.f);
	m_pMainHall->GetTransform()->Translate(-1.f, -24.f, 23.f);
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
	auto& pCamManager = ReCameraManager::Get();

	XMFLOAT3 camPos{ -35.f, 32.0f, -43.0f };
	XMVECTOR camUp{ 0.166378f, 0.971491f, 0.168888f };
	XMVECTOR camLook{ 0.681785f,  -0.237076f, 0.692072f };
	float fov = 0.703f;

	XMFLOAT4 lOrientation{46.6f, -290.3f, 168.f, 1.f};
	XMFLOAT4 lPosition{-18.1f, 70.f, -55.8f, 1.f};

	auto reCam = new ReCamera(camPos);
	auto cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	camPos = { 0.f, 0, -0 };
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	pCamManager.SetActiveCamera(UINT(0));
}

void MainHallScene::AddCameraSwitches()
{
	XMFLOAT3 pos = { 0, 0, 0 };
	XMFLOAT3 size = { 1, 1, 1 };
	auto pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(0, 1);
	m_pSwitches.push_back(pSwitch);


	pSwitch = AddChild(new CameraSwitch({ 0, 0, 0 }, { 1, 1, 1 }, true));
	m_pSwitches.push_back(pSwitch);
}

void MainHallScene::AddDoors()
{
	XMFLOAT3 pos{ 0.f, 0.f, 0.f };
	XMFLOAT3 size{ 1.f, 1.f,1.f };

	pos = { -56.0f, 11.0f, 26.0f };
	size = { 7.0f, 31.0f, 29.0f };
	auto pDoor = AddChild(new ReDoor(pos, size));
	pDoor->SetSceneToLoad(1);
	m_pDoors.emplace_back(pDoor);

	pos = { -56.0f, 61.0f, 26.0f };
	pDoor = AddChild(new ReDoor(pos, size));
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
	m_pNavCollider = AddChild(new GameObject());
	auto pNavTriangleMesh = ContentManager::Load<PxTriangleMesh>(FilePath::ENV_MAIN_NAVMESH);
	auto pNavRb = m_pNavCollider->AddComponent(new RigidBodyComponent(true));
	pNavRb->SetCollisionGroup(CollisionGroup::Group0);
	pNavRb->AddCollider(PxTriangleMeshGeometry(pNavTriangleMesh), material);
	m_pNavCollider->GetTransform()->Scale(15.f, 15.f, 15.f);
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
	m_pCharacter->GetTransform()->Translate(0.f, characterDesc.controller.height / 2.f + 5, 0.f);
}


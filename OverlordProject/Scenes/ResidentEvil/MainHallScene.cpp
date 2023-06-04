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
#include "ResidentEvil/World/ThunderController.h"
#include "Prefabs/CubePrefab.h"
#include "ResidentEvil/World/ReClassicDoor.h"
#include "ResidentEvil/HUD/SubtitleManager.h"
#include "Materials/Post/PostGrain.h"
#include "Materials/Post/PostBloom.h"
#include "ResidentEvil/HUD/ReMenuManager.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"
#include "ResidentEvil/ReGameManager.h"
#include "ResidentEvil/Items/ReGun.h"

MainHallScene::MainHallScene()
	: ReScene(L"MainHallScene")
{
}

void MainHallScene::Initialize()
{
	m_SceneContext.useDeferredRendering = true;


	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.pLights->GetDirectionalLight().isEnabled = true;
	const auto pDefaultMaterial = PxGetPhysics().createMaterial(0.5f, 0.5f, 0.5f);
	m_pFMODSys = SoundManager::Get()->GetSystem();

	m_pClassicDoor = AddChild(new ReClassicDoor());

	m_pThunderController = AddChild(new ThunderController());
	LoadMainMenu();
	AddLights();
	AddDoors();
	//AddCameraSwitches();
	AddInput();
	AddNavCollider(*pDefaultMaterial);
	AddPlayer(pDefaultMaterial);
	AddPostProcessing();

	AddSound();
}

static int m_SelectedLight;
void MainHallScene::Update()
{
	if (m_pDebugCube)
	{
		const auto& selectedLightPos = m_SceneContext.pLights->GetLights()[m_SelectedLight].position;
		m_pDebugCube->GetTransform()->Translate(selectedLightPos.x, selectedLightPos.y, selectedLightPos.z);
	}

	if (m_SceneContext.pInput->IsActionTriggered(ResetScene))
		Reset();

	if (m_SceneContext.pInput->IsActionTriggered(MenuUp))
		m_pMenuManager->SwitchMenu(ReMenuType::INGAME);
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
	myfile << "LookAt: " << lookat.x << "f, " << lookat.y << "f, " << lookat.z << "f\n";

	// print right
	XMFLOAT3 right;
	XMStoreFloat3(&right, cam.GetRight());
	myfile << "Right: " << right.x << " " << right.y << " " << right.z << "\n";

	//print up
	XMFLOAT3 up;
	XMStoreFloat3(&up, cam.GetUp());
	myfile << "Up: " << up.x << "f, " << up.y << "f, " << up.z << "f\n";

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

			ReCameraManager::Get().SetActiveCamera(UINT(camIdx));
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

			if (light.type == LightType::Spot)
			{
				ImGui::SliderFloat("SpotAngle", &light.spotLightAngle, 0, 180);

				// direction
				float direction[3] = { light.direction.x, light.direction.y, light.direction.z };
				ImGui::DragFloat3("Direction", direction, .2f);
				light.direction = { direction[0], direction[1], direction[2], 1.0f };
			}

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

void MainHallScene::Start()
{
	LoadWorld();
	Reset();

	m_pThunderController->Enable();
	m_pAmbientChannel->setPaused(false);
	m_pFMODSys->playSound(m_pAmbientSound, nullptr, false, &m_pAmbientChannel);
	ReCameraManager::Get().SetActiveCamera(UINT(0));


	SceneManager::Get()->SetActiveGameScene(L"MainHallScene");
}

void MainHallScene::Reset()
{
	m_pCharacter->Reset();
	m_pGun->Reset();
	ReCameraManager::Get().SetActiveCamera(static_cast<UINT>(ReMainHallCamera::MAIN));
}

void MainHallScene::AddMenu()
{
	//auto pMenuManager = AddChild(new GameObject());
	
}

void MainHallScene::AddGun()
{
	//m_pGun = AddChild(new ReGun({ 8,10,-63 }, { -15,29,8 }));
	//m_pGun->SetDestInventory(m_pCharacter->GetInventory());
}

void MainHallScene::AddSound()
{
	const auto& ambientPath = ContentManager::GetFullAssetPath(FilePath::MAINHALL_AMBIENT_AUDIO);
	auto result = m_pFMODSys->createStream(ambientPath.string().c_str(), FMOD_LOOP_NORMAL, nullptr, &m_pAmbientSound);
	if (result != FMOD_OK)
	{
		Logger::LogError(L"Failed to load sound: " + FilePath::MAINHALL_AMBIENT_AUDIO);
		return;
	}

}

void MainHallScene::LoadMainMenu()
{
	// Background
	const float centerWidth = m_SceneContext.windowWidth * .5f;
	const float centerHeight = m_SceneContext.windowHeight * .5f;

	auto pBackground = AddChild(new GameObject());
	m_pBackground = pBackground->AddComponent(new SpriteComponent(FilePath::MAINMENU_BACKGROUND_IMG, { 0.5f, 0.5f }, { 1.f, 1.f, 1.f, 1.f }));
	pBackground->GetTransform()->Translate(centerWidth, centerHeight, 0.f);

	m_pMenuManager = AddChild(new ReMenuManager());
	const float margin{ 35.f };
	const float offset{ 150.f };
	const XMFLOAT2 btnSize{ 75, 25 };
	SpriteFont* pFont = ContentManager::Load<SpriteFont>(FilePath::SUB_FONT);

	// Main 
	auto pMainMenu = AddChild(new ReMenu(ReMenuType::INGAME));
	pMainMenu->GetTransform()->Scale(1.f, 1.f, 1.f);

	auto pToMainBtn = new ReButton({ centerWidth - btnSize.x * .5f, centerHeight + offset }, btnSize, pFont);
	pToMainBtn->AddOnClick([this]() { ReGameManager::Get().StartScene(ReScenes::MENU); });
	pToMainBtn->SetText("MAIN MENU");

	auto pRestart = new ReButton({ centerWidth - btnSize.x * .7f, centerHeight + offset + margin }, btnSize, pFont);
	pRestart->SetText("Reset");
	pRestart->AddOnClick([this]() { Reset(); });

	auto pExitBtn = new ReButton({ centerWidth - btnSize.x * .5f, centerHeight + offset + margin * 2 }, btnSize, pFont);
	pExitBtn->AddOnClick([this]() {
		std::cout << "Exit\n";
		});
	pExitBtn->SetText("EXIT");

	pMainMenu->AddButton(pToMainBtn);
	pMainMenu->AddButton(pExitBtn);
	pMainMenu->AddButton(pRestart);
	pMainMenu->AddImage(m_pBackground);
	m_pMenuManager->AddMenu(pMainMenu);


	m_pMenuManager->DisableMenus();

}
void MainHallScene::LoadWorld()
{
	if (m_WorldLoaded) {
		ReCameraManager::Get().Reset();
		AddCameras();
		return;
	};

	auto pOccluder = AddChild(new GameObject());
	pOccluder->AddComponent(new ModelComponent(FilePath::ENV_OCCLUDER));

	m_pMainHall = AddChild(new GameObject());
	std::wstring mainhall_folder = ContentManager::GetFullAssetPath(FilePath::FOLDER_ENV_MAINHALL);
	if (m_SceneContext.useDeferredRendering)
		StaticMeshFactory::Get()->AddMtlModelComponent<BasicMaterial_Deferred_Shadow>(*m_pMainHall, mainhall_folder);
	else
		StaticMeshFactory::Get()->AddMtlModelComponent<DiffuseMaterial_Shadow>(*m_pMainHall, mainhall_folder);
	m_pMainHall->GetTransform()->Scale(100.f, 100.f, 100.f);
	m_pMainHall->GetTransform()->Translate(-1.f, -24.f, 23.f);

	m_WorldLoaded = true;
}

void MainHallScene::AddLights()
{
	m_SceneContext.pLights->SetDirectionalLight({ 0, 56.6f, 62.4f }, { -.187f, -0.017f, -.993f });

	Light light{};
	XMFLOAT4 color{ 1.580f, 1.54f, 1.18f, 1.f };

	// Main hall light
	light.type = LightType::Point;
	light.position = { 1.2f, 81.2f, 14.8f, 1 };
	light.color = color;
	light.intensity = .981f;
	light.range = 100.f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);


	// Main candle right
	light.type = LightType::Point;
	light.position = { -14.8f, 29.f, 33.4f, 1 };
	light.color = color;
	light.intensity = .905f;
	light.range = 41.905f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);

	// Main candle left
	light.type = LightType::Point;
	light.position = { 1.580f, 1.54f, 1.18f, 1 };
	light.color = color;
	light.intensity = .0f;
	light.range = 10.f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);

	// Door light R1
	light.type = LightType::Spot;
	light.position = { 54.2f, 28.f, 62.2f, 1.f };
	light.direction = { 0.f, -1.f, 0.f, 1.f };
	light.spotLightAngle = 0.785398f;
	light.color = color;
	light.intensity = .9f;
	light.range = 37.143f;
	light.isEnabled = true;
	m_SceneContext.pLights->AddLight(light);

	// Door light R2
	light.type = LightType::Point;
	light.position = { 54.2f, 29.f, 11.2f, 1.f };
	light.color = color;
	light.intensity = .352f;
	light.range = 43.81f;
	light.isEnabled = false;
	m_SceneContext.pLights->AddLight(light);

	// Stair light
	light.type = LightType::Spot;
	light.position = { 0.2f, 0.f, 0, 1 };
	light.color = color;
	light.direction = { 0.f, -1.f, 0.f, 1.f };
	light.spotLightAngle = 0.785398f;
	light.intensity = .0f;
	light.range = 10.f;
	light.isEnabled = false;
	m_SceneContext.pLights->AddLight(light);

	// CT1 
	light.type = LightType::Point;
	light.position = { 54.2f, 29.f, 11.2f, 1.f };
	light.color = { -31.62f, 13.34f, 88.78f, 1.f };
	light.intensity = .352f;
	light.range = 43.81f;
	light.isEnabled = false;
	m_SceneContext.pLights->AddLight(light);

	// CT2
	light.type = LightType::Point;
	light.position = { 34.98f, 13.34f, 88.78f, 1.f };
	light.color = { 1.68f, 1.66f, 2.f, 1.f };
	light.intensity = .962f;
	light.range = 21.905f;
	light.isEnabled = false;
	m_SceneContext.pLights->AddLight(light);
}

void MainHallScene::AddCameras()
{
	auto& pCamManager = ReCameraManager::Get();

	XMFLOAT3 camPos{ -35.f, 32.0f, -43.0f };
	XMVECTOR camUp{ 0.166378f, 0.971491f, 0.168888f };
	XMVECTOR camLook{ 0.681785f,  -0.237076f, 0.692072f };
	float fov = 0.703f;

	XMFLOAT4 lOrientation{ 46.6f, -290.3f, 168.f, 1.f };
	XMFLOAT4 lPosition{ -18.1f, 70.f, -55.8f, 1.f };

	// [0] Main
	auto reCam = new ReCamera(camPos);
	auto cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [1] Dining 
	camPos = { 46.f, 7, -19 };
	camLook = { -0.952282f, 0.178562f, 0.247536f };
	camUp = { 0.172819f, 0.983929f, -0.0449225f };
	lOrientation = { -16.2f, -11.3f, 6.5f, 1.0f };
	lPosition = { 60.9f, 65.4f, 0.f, 1.0f };
	reCam = new ReCamera(camPos);
	fov = .868f;
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [2] CT1
	camLook = { -0.350739f, 0.0598116f, 0.934561f };
	camUp = { 0.0210159f, 0.99821f, -0.0559979f };
	camPos = { -27.f, 11.f, -10.f };
	lOrientation = { -1.5f, -0.43f, 4.2f, 1.0f };
	lPosition = { 1.5f, 42.8f, -0.9f, 1.0f };
	fov = .688f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [3] Stairs
	camLook = { -0.163124f, 0.406737f, 0.898864f };
	camUp = { 0.0726276f, 0.913546f, -0.4002f };
	camPos = { 10.f, 0.f, 11.1f };
	lOrientation = { 0.f, 2.9f, 17.1f, 1.0f };
	lPosition = { 0.f, 8.3f, .0f, 1.0f };
	fov = .494f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [5] Entrance
	camPos = { -10.f, 66.f, 9.f };
	camLook = { 0.21046f, -0.61172f, -0.762564f };
	camUp = { 0.162744f, 0.791074f, -0.589675f };
	lOrientation = { -74.1f, -1.6f, -40.1f, 1.0f };
	lPosition = { 0.f, 14.3f, .0f, 1.0f };
	fov = .703f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [6] CT2
	camLook = { 0.459824f, 0.0598116f, 0.885993f };
	camUp = { -0.0275522f, 0.99821f, -0.0530877f };
	camPos = { 21, 9, 0 };
	lOrientation = { 0.9f, -1.43f, 7.0f, 1.0f };
	lPosition = { 1.5f, 42.8f, -0.9f, 1.0f };
	fov = .688f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);


	// [7] Stairs Mid
	camLook = { -0.804462f, -0.406737f, 0.432904f };
	camUp = { -0.35817f, 0.913545f, 0.192741f };
	camPos = { 26, 51, 69 };
	lOrientation = { 6.4f, -3.3f, 7.2f, 1.0f };
	lPosition = { -37.5f, 75.3f, 80.9f, 1.0f };
	fov = .755f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [8] Upper Main
	camLook = { -0.82755f, -0.294758f, -0.477786f };
	camUp = { -0.255268f, 0.955572f - 0.147379f };
	camPos = { 60, 72, 81 };
	lOrientation = { -1.2f, -.7f, 1.5f, 1.0f };
	lPosition = { 0.f, 80.4f, 0.f, 1.0f };
	fov = .898f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [8] Upper Dining
	camLook = { -0.777137f - 0.406737f, 0.480232f };
	camUp = { -0.346004f, 0.913545f, 0.213813f };
	camPos = { -14, 78 ,-29 };
	lOrientation = { -1.4f, -.8f, 1.5f, 1.5f };
	lPosition = { 16.9f, 63.9f, 3.1f, 1.0f };
	fov = .770f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [9] Stair Bridge
	camLook = { 0.970635f, -0.178562f, -0.161192f };
	camUp = { 0.176149f, 0.983929f, -0.0292529f };
	camPos = { -32, 68, 71 };
	lOrientation = { 12.2f, -4.3f, 8.6f, 1.5f };
	lPosition = { -10.3f, 85.2f, 0.8f, 1.0f };
	fov = .770f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [10] Right Corner
	camLook = { 0.764587f, -0.294758f, -0.573171f };
	camUp = { 0.235846f, 0.955572f, -0.176802f };
	camPos = { -2 ,79, 8 };
	lOrientation = { 12.2f, -4.3f, 8.6f, 1.5f };
	lPosition = { -10.3f, 85.2f, 0.8f, 1.0f };
	fov = .793f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);

	// [11] Window 
	camLook = { 0.649952f, -0.294757f, -0.700485f };
	camUp = { 0.200485f, 0.955572f, -0.216073f };
	camPos = { -47, 76, 2 };
	lOrientation = { -21.5f, -10.6f, -49.6f, 1.5f };
	lPosition = { 38.9f, 66.3f, 3.6f, 1.0f };
	fov = .778f;
	reCam = new ReCamera(camPos);
	cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	reCam->SetLightOrientation(lOrientation);
	reCam->SetLightPosition(lPosition);
	cam->UpdateRotation(m_SceneContext, camUp, camLook);
	AddChild(reCam);
	pCamManager.AddVolume(reCam);
}

#define SC_UINT(x) static_cast<UINT>(x)
void MainHallScene::AddCameraSwitches()
{
	// Main <-> CT2
	XMFLOAT3 pos = { 43.8f, 9.6f, 28.8f };
	XMFLOAT3 size = { 43, 21, 2 };
	auto pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTargets(SC_UINT(ReMainHallCamera::MAIN), SC_UINT(ReMainHallCamera::CT2));
	pSwitch->GetTransform()->Rotate(0, 29, 0);
	m_pSwitches.push_back(pSwitch);


	// -> Stairs
	pos = { 43.8f, 9.6f, 28.8f };
	size = { 43, 21, 2 };
	pSwitch = AddChild(new CameraSwitch(pos, size));
	pSwitch->SetTarget(SC_UINT(ReMainHallCamera::STAIRS));
	pSwitch->GetTransform()->Rotate(0, 29, 0);


	pSwitch = AddChild(new CameraSwitch({ 0, 0, 0 }, { 1, 1, 1 }, true));
	m_pSwitches.push_back(pSwitch);
}

void MainHallScene::AddDoors()
{
	XMFLOAT3 pos{ 0.f, 0.f, 0.f };
	XMFLOAT3 size{ 1.f, 1.f,1.f };

	// To Dining
	pos = { -56.0f, 11.0f, 26.0f };
	size = { 7.0f, 31.0f, 29.0f };
	auto pDoor = AddChild(new ReDoor(pos, size));
	pDoor->OnInteract.AddFunction([this]()
		{
			ReGameManager::Get().SetSpawnPos(SpawnPositions::Dining_Bottom);
			m_pClassicDoor->SetSceneToLoad(ReScenes::DINING);
			m_pClassicDoor->Trigger();
		});

	m_pDoors.emplace_back(pDoor);

	// To Upper Dining
	pos = { -56.0f, 61.0f, 26.0f };
	pDoor = AddChild(new ReDoor(pos, size));
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
	inputAction = InputAction(CharacterAim, InputState::down, VK_RBUTTON);
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(ResetScene, InputState::down, 'P');
	m_SceneContext.pInput->AddInputAction(inputAction);
	inputAction = InputAction(MenuUp, InputState::down, 'M');
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
	characterDesc.actionId_Interact = Interact;
	characterDesc.actionId_Sprint = CharacterSprint;
	characterDesc.actionId_Aim = CharacterAim;
	characterDesc.actionId_Attack = CharacterAttack;
	characterDesc.controller.height = 17.5f;
	characterDesc.rotationSpeed = 120.f;
	characterDesc.maxMoveSpeed = 15.f;
	characterDesc.moveAccelerationTime = 0.01f;
	//characterDesc.spawnPosition = m_SpawnPos = ReGameManager::Get().GetSpawnPos();
	characterDesc.spawnPosition = m_PlayerStartPos;
	m_pCharacter = AddChild(new RePlayerController(characterDesc));
}


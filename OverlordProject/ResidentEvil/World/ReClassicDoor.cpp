#include "stdafx.h"
#include "ReClassicDoor.h"
#include "FilePaths.h"
#include "Materials/Deferred/BasicMaterial_Deferred_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Utils/StaticMeshFactory.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ThunderController.h"
#include "ResidentEvil/ReData.h"
#include "ResidentEvil/ReGameManager.h"

ReClassicDoor::ReClassicDoor()
	: m_TriggerOpen{ false }
	, m_Duration{ 8.f } // length of original door animation :)
	, m_TotalYaw{ .0f }
	, m_StartYaw{ 0 }
	, m_EndYaw{ 90 }
{
}

ReClassicDoor::ReClassicDoor(float duration, float startYaw, float endYaw)
	: m_TriggerOpen{false}
	, m_Duration{duration}
	, m_TotalYaw{.0f}
	, m_StartYaw{startYaw}
	, m_EndYaw{endYaw}
	, m_SceneToLoad{ReScenes::MAIN}
{
}

void ReClassicDoor::Initialize(const SceneContext& sceneContext)
{
	auto pDoorModel = AddComponent(new ModelComponent(ContentManager::GetFullAssetPath(FilePath::DOOR_MESH)));
	if (sceneContext.useDeferredRendering)
	{
		auto pMaterial = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred_Shadow>();
		pMaterial->SetDiffuseTexture(FilePath::DOOR_DIFFUSE);
		pMaterial->SetNormalMap(FilePath::DOOR_NORMAL);
		pDoorModel->SetMaterial(pMaterial);
	} 
	else
	{
		auto pMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
		pMaterial->SetDiffuseTexture(FilePath::DOOR_DIFFUSE);
		pMaterial->SetNormalMap(FilePath::DOOR_NORMAL);
		pDoorModel->SetMaterial(pMaterial);
	}

	// add door cam
	auto& pCamManager = ReCameraManager::Get();

	XMFLOAT3 camPos{ 0.f,-90.f, -30.f };
	XMVECTOR camUp{ 0.f, 1.f, 0.f };
	XMVECTOR camLook{ 0.f,  0.f, 1.f };

	float fov = 0.703f;
	m_pCamera = new ReCamera(camPos);
	auto cam = m_pCamera->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(sceneContext, camUp, camLook);
	sceneContext.pCamera->GetScene()->AddChild(m_pCamera);
	m_CamID = pCamManager.AddVolume(m_pCamera);

	GetTransform()->Translate(1.f, -100.f, 1.f);
	pDoorModel->GetTransform()->Scale(.1f, .1f, .1f);

	auto pFMODSys = SoundManager::Get()->GetSystem();
	auto doorSoundPath = ContentManager::GetFullAssetPath(FilePath::DOOR_AUDIO);
	auto result = pFMODSys->createStream(doorSoundPath.string().c_str(), FMOD_2D, nullptr, &m_pDoorAnimSound);
	if (result != FMOD_OK)
	{
		std::wstringstream ss;
		ss << L"Failed to load door sound: " << doorSoundPath << L"\n";
		Logger::LogError(ss.str());
	}

	OnAnimationStart.AddFunction([this]()
	{
		SoundManager::Get()->GetSystem()->playSound(m_pDoorAnimSound, nullptr, false, &m_pDoorChannel);
	});
}

void ReClassicDoor::Update(const SceneContext& sceneContext)
{
	if (!m_TriggerOpen)
		return;

	const float dt = sceneContext.pGameTime->GetElapsed();
	m_TimePassed += dt;
	UpdateKeyframeEvents();
	GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);


	if (!m_CamMoved && m_TimePassed < m_DoorOpenTime)
	{
		const auto& camPos = m_pCamera->GetTransform()->GetPosition();
		m_pCamera->GetTransform()->Translate(camPos.x, camPos.y, camPos.z + 3 * dt);
	}

	if (m_TimePassed > m_DoorFullOpenTime)
	{
		const auto& camPos = m_pCamera->GetTransform()->GetPosition();
		m_pCamera->GetTransform()->Translate(camPos.x, camPos.y, camPos.z + 15 * dt);
	}

	if (m_OpenDoor)
	{
		float t = m_TimePassed - m_DoorOpenTime / m_TimePassed - m_DoorFullOpenTime;
		t = std::clamp(t, 0.0f, 1.0f); // Ensure t is within the range [0, 1]

		m_TotalYaw = m_StartYaw + (m_EndYaw - m_StartYaw) * t;
	}

	if (m_TimePassed > m_Duration)
	{
		if(m_SceneToLoad != ReScenes::ZERO) ReGameManager::Get().StartScene(m_SceneToLoad);
		OnAnimationFinished.Invoke();
		Reset();
	}
}

void ReClassicDoor::Trigger()
{
	m_TriggerOpen = true;
	ReCameraManager::Get().SetActiveCamera(m_CamID);

	m_TotalYaw = 0;
	m_pCamera->GetTransform()->Translate(0.f, -90.f, -30.f);
	OnAnimationStart.Invoke(); // watch out for unwanted invokes
}

void ReClassicDoor::SetSceneToLoad(const ReScenes& scene)
{
	m_SceneToLoad = scene;
}

void ReClassicDoor::Reset()
{
	m_AnimationOnly = false;
	m_TriggerOpen = false;
	m_TimePassed = 0.0f;
	m_OpenDoor = false;
	m_CamMoved = false;
	m_TotalYaw = 0;
	m_pCamera->GetTransform()->Rotate(0.f, 0.f, 0.f);
	m_pCamera->GetTransform()->Translate(0.f, -90.f, -30.f);
	OnAnimationFinished.Clear();
}

void ReClassicDoor::ClearDelegates()
{
	OnAnimationStart.Clear();
	OnAnimationFinished.Clear();


	OnAnimationStart.AddFunction([this]()
		{
			SoundManager::Get()->GetSystem()->playSound(m_pDoorAnimSound, nullptr, false, &m_pDoorChannel);
		});
}

void ReClassicDoor::UpdateKeyframeEvents()
{
	if (m_TimePassed >= m_StartMoveCamTime && !m_CamMoved)
	{
		m_CamMoved = true;
	}

	if (m_TimePassed >= m_DoorOpenTime && m_TimePassed <= m_DoorFullOpenTime)
	{
		m_OpenDoor = true;
	}

	if (m_TimePassed >= m_DoorCloseSFXtime && !m_DoorCloseSFXplayed)
	{
		// play door close sound
	}
}

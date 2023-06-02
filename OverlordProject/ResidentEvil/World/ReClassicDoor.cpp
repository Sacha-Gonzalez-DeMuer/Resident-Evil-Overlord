#include "stdafx.h"
#include "ReClassicDoor.h"
#include "FilePaths.h"
#include "Materials/Deferred/BasicMaterial_Deferred_Shadow.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Utils/StaticMeshFactory.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "ResidentEvil/Camera/ReCamera.h"

ReClassicDoor::ReClassicDoor()
	: m_TriggerOpen{ false }
	, m_Duration{ 8.f } // length of original door animation :)
	, m_TotalYaw{ .0f }
	, m_StartYaw{ 0 }
	, m_EndYaw{ 90 }
	, m_AnimLerp{ 0.f }
{
}

ReClassicDoor::ReClassicDoor(float duration, float startYaw, float endYaw)
	: m_TriggerOpen{false}
	, m_Duration{duration}
	, m_TotalYaw{.0f}
	, m_StartYaw{startYaw}
	, m_EndYaw{endYaw}
	, m_AnimLerp{.0f}
{
}

void ReClassicDoor::Initialize(const SceneContext& sceneContext)
{
	auto pDoor = sceneContext.pCamera->GetScene()->AddChild(new GameObject());
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

	XMFLOAT3 camPos{ -10.f,-100.f, 1.0f };
	XMVECTOR camUp{ 0.f, 1.f, 0.f };
	XMVECTOR camLook{ 1.f,  0.f, 0.f };

	float fov = 0.703f;
	auto reCam = new ReCamera(camPos);
	auto cam = reCam->GetCamera();
	cam->SetFieldOfView(fov);
	cam->UpdateRotation(sceneContext, camUp, camLook);
	sceneContext.pCamera->GetScene()->AddChild(reCam);
	m_CamID = pCamManager.AddVolume(reCam);

	pDoor->GetTransform()->Translate(1.f, -100.f, 1.f);

	OnAnimationFinished.AddFunction([this]()
		{
		SceneManager::Get()->SetActiveGameScene(m_SceneToLoad);
	});

}

void ReClassicDoor::Update(const SceneContext& sceneContext)
{
	if (!m_TriggerOpen) return;

	if (m_TimePassed > m_Duration)
	{
		m_AnimLerp = 0.f;
		m_TriggerOpen = false;
		OnAnimationFinished.Invoke();
		return;
	}

	m_TimePassed += sceneContext.pGameTime->GetElapsed();
	UpdateKeyframeEvents();
		
	m_TotalYaw = m_StartYaw + (m_EndYaw - m_StartYaw) * m_AnimLerp;
	GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);

}

void ReClassicDoor::Trigger()
{
	m_TriggerOpen = true;
	ReCameraManager::Get().SetActiveCamera(m_CamID);
}

void ReClassicDoor::UpdateKeyframeEvents()
{
	if (m_TimePassed >= m_DoorHingeSFXtime && !m_DoorHingeSFXplayed)
	{
		// play door hinge sound
		m_DoorHingeSFXplayed = true;
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

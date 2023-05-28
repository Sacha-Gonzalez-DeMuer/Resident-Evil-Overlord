#include "stdafx.h"
#include "ReCamera.h"
#include "Components/CameraComponent.h"
#include "ReCameraManager.h"
#include "Prefabs/CubePrefab.h"

// constructor
ReCamera::ReCamera(const DirectX::XMFLOAT3& position, bool targetable)
	: m_TotalPitch{0}
    , m_TotalYaw{0}
    , m_TotalRoll{0}
{
    InitializeCamera(position, targetable);
}

float& ReCamera::GetFOV()
{ 
    return m_pCamera->GetFOV();
}

void ReCamera::ActivateCam()
{
    m_idx = ReCameraManager::Get().AddVolume(this);
    ReCameraManager::Get().SetActiveCamera(m_idx);
}

void ReCamera::Update(const SceneContext& /*sceneContext*/)
{
    GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, m_TotalRoll);
}

void ReCamera::OnCameraSwitch()
{
    if (m_HasLight)
        GetScene()->GetSceneContext().pLights->SetDirectionalLight(GetTransform()->GetPosition(), m_LightOrientation);
}

void ReCamera::InitializeCamera(const XMFLOAT3& position, bool targetable)
{   
    m_pCamera = AddComponent(new CameraComponent(targetable));
    GetTransform()->Translate(position);
}
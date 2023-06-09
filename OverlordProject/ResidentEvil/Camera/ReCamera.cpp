#include "stdafx.h"
#include "ReCamera.h"
#include "Components/CameraComponent.h"
#include "ReCameraManager.h"
#include "Prefabs/CubePrefab.h"

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

void ReCamera::Update(const SceneContext& sceneContext)
{
    if (m_pCamera->IsActive())
    {
        GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, m_TotalRoll);

        if (m_HasLight)
        {
            sceneContext.pLights->GetDirectionalLight().direction = m_LightOrientation;
            sceneContext.pLights->GetDirectionalLight().position = m_LightPosition;
        }
       // else sceneContext.pLights->ResetDirectionalLight();
    }
}

void ReCamera::OnCameraSwitch()
{
    auto pLights = GetScene()->GetSceneContext().pLights;
    if (m_HasLight)
    {
        pLights->SetDirectionalLight({m_LightPosition.x, m_LightPosition.y, m_LightPosition.z}, {m_LightOrientation.x, m_LightOrientation.y, m_LightOrientation.z});
    }
    else pLights->ResetDirectionalLight();
}

void ReCamera::InitializeCamera(const XMFLOAT3& position, bool targetable)
{   
    m_pCamera = AddComponent(new CameraComponent(targetable));
    GetTransform()->Translate(position);
}
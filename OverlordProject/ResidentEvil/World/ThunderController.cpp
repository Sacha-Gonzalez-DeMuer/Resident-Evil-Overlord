#include "stdafx.h"
#include "ThunderController.h"
#include <random>

void ThunderController::Update(const SceneContext& sceneContext)
{
    auto& pLight = sceneContext.pLights->GetDirectionalLight();
	const auto& deltaTime = sceneContext.pGameTime->GetElapsed();


    if (m_isLightOn)
    {
        m_CurrentFlashDuration -= deltaTime;

        if (m_CurrentFlashDuration <= 0)
        {
            m_isLightOn = false;
            m_CurrentFlashDuration = MathHelper::randF(m_MinFlashDuration, m_MaxFlashDuration);
        }
    }
    else
    {
        m_DelayTimer += deltaTime;

        if (m_DelayTimer > m_CurrentDelay)
        {
            m_isLightOn = true;
            m_CurrentDelay = MathHelper::randF(m_MinDelay, m_MaxDelay);
            m_DelayTimer = 0;
        }
    }

    float intensity = MathHelper::randF(m_MinIntensity, m_MaxIntensity);
    pLight.intensity = m_isLightOn 
        ? intensity : m_MinIntensity;


}

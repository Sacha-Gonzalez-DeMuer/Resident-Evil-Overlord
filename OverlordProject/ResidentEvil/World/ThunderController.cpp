#include "stdafx.h"
#include "ThunderController.h"
#include <random>

void ThunderController::Update(const SceneContext& sceneContext)
{
    auto pLight = sceneContext.pLights->GetDirectionalLight();

	const auto& deltaTime = sceneContext.pGameTime->GetElapsed();
   if (!pLight.isEnabled && m_CurrentDelay <= 0) {
       // Enable light
       pLight.isEnabled = true;
       m_CurrentFlashDuration = m_FlashDuration;
   }
   else if (pLight.isEnabled && m_CurrentFlashDuration <= 0) {
       // Disable light
       pLight.isEnabled = false;

       m_CurrentDelay = MathHelper::randF(m_MinDelay, m_MaxDelay);
   }
   else if (m_CurrentDelay > 0) {
       m_CurrentDelay -= deltaTime;
   }
   else {
       m_CurrentFlashDuration -= deltaTime;
   }
}

#include "stdafx.h"
#include "RePlayerAnimController.h"
#include "ResidentEvil/Player/RePlayerController.h"
#include "ResidentEvil/ReGameManager.h"
RePlayerAnimController::RePlayerAnimController(ModelAnimator* modelAnimator, RePlayerController* player)
	: m_pAnimator{ modelAnimator }, m_pPlayerController{ player }
{
	
}

void RePlayerAnimController::Initialize(const SceneContext& /*sceneContext*/)
{
}

void RePlayerAnimController::Update(const SceneContext& sceneContext)
{
	if (m_pPlayerController->IsDead() && m_DeathTimer > 0)
	{
		m_DeathTimer -= sceneContext.pGameTime->GetElapsed();

		if (m_DeathTimer <= 0)
			ReGameManager::Get().StartScene(ReScenes::DEATH);
	}
	if (m_Pause) return;

	using namespace PAnimNames;
	const auto& currentAnim = m_pAnimator->GetClipName();

	if (currentAnim == Bitten)
	{
		m_BloodTimer += sceneContext.pGameTime->GetElapsed();
		if (m_BloodTimer > m_BloodDelay)
		{
			m_pBloodEmitter->Play();
		}
	}
	else
	{
		m_BloodTimer = 0;
		m_pBloodEmitter->Stop();
	}


	if (m_pPlayerController->IsAnimLocked() || m_pPlayerController->IsDead()) return;

	auto pInput = sceneContext.pInput;
	const auto& playerDesc = m_pPlayerController->GetDesc();

	bool isSprinting = m_pPlayerController->IsSprinting();
	bool isMoving = m_pPlayerController->IsMoving();
	bool isMovingBackward = pInput->IsActionTriggered(playerDesc->actionId_MoveBackward);
	bool isAiming = m_pPlayerController->IsAiming();
	m_pAnimator->SetPlayReversed(false);

	if (isSprinting && isMoving)
	{
		if (currentAnim != Sprint)
			m_pAnimator->SetAnimation(Sprint);
	}
	else if (isMoving)
	{
		m_pAnimator->SetPlayReversed(isMovingBackward);
		if (currentAnim != Walk)
			m_pAnimator->SetAnimation(Walk);
	}
	else if (isAiming)
	{
		if (currentAnim != PistolIdle)
			m_pAnimator->SetAnimation(PistolIdle);
	}
	else
	{
		if (currentAnim != Idle) 
			m_pAnimator->SetAnimation(Idle);
	}
}

void RePlayerAnimController::TriggerAnim(const std::wstring& clipName)
{
	const auto& currentAnim = m_pAnimator->GetClipName();
	if (currentAnim != clipName)
	{
		m_pAnimator->PlayOnce(clipName, PAnimNames::Idle);
	}
}

void RePlayerAnimController::TriggerAnim(const std::wstring& clipName, const std::wstring& nextClipName, std::function<void()> callback)
{
	const auto& currentAnim = m_pAnimator->GetClipName();
	if (currentAnim != clipName)
	{
		m_pAnimator->PlayOnce(clipName, nextClipName, callback);
	}
}

void RePlayerAnimController::ClearNextAnim()
{
	m_pAnimator->ClearNextAnim();
}

void RePlayerAnimController::Pause()
{
	m_Pause = true;
	m_pAnimator->Pause();
}

void RePlayerAnimController::Reset()
{
	m_pAnimator->Reset();
	m_Pause = false;
	m_DeathTimer = m_DeathAnimTime;
}

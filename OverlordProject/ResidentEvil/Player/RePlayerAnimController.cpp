#include "stdafx.h"
#include "RePlayerAnimController.h"
#include "ResidentEvil/Player/RePlayerController.h"

RePlayerAnimController::RePlayerAnimController(ModelAnimator* modelAnimator, RePlayerController* player)
	: m_pAnimator{ modelAnimator }, m_pPlayerController{ player }
{
	
}

void RePlayerAnimController::Initialize(const SceneContext& /*sceneContext*/)
{
}

void RePlayerAnimController::Update(const SceneContext& sceneContext)
{
	using namespace PAnimNames;
	const auto& currentAnim = m_pAnimator->GetClipName();

	if (currentAnim == Bitten)
	{
		m_pBloodEmitter->Play();
		m_pBloodEmitter->GetTransform()->Translate(m_pPlayerController->GetTransform()->GetWorldPosition());
	}
	else if (m_pBloodEmitter->IsPlaying())
	{
	m_pBloodEmitter->Stop();
	}


	if (m_pPlayerController->IsAnimLocked()) return;

	auto pInput = sceneContext.pInput;
	const auto& playerDesc = m_pPlayerController->GetDesc();

	bool isSprinting = pInput->IsActionTriggered(playerDesc->actionId_Sprint);
	bool isMoving = m_pPlayerController->IsMoving();
	bool isMovingBackward = pInput->IsActionTriggered(playerDesc->actionId_MoveBackward);
	bool isAiming = m_pPlayerController->IsAiming();
	m_pAnimator->SetPlayReversed(false);

	// inb4 yandere dev - no time to make a state machine :(
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

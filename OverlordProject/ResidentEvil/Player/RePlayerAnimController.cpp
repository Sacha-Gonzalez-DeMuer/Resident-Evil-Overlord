#include "stdafx.h"
#include "RePlayerAnimController.h"
#include "ResidentEvil/Player/RePlayerController.h"

namespace AnimNames {
	const std::wstring Idle{ L"Idle" };
	const std::wstring Walk{ L"Walk" };
	const std::wstring Sprint{ L"Running" };
	const std::wstring PistolIdle{ L"Pistol_Idle" };
	const std::wstring PistolWalk{ L"Pistol_Walk" };
	const std::wstring OpenDoor{ L"OpenDoor" };
	const std::wstring ItemPickup{ L"ItemPickup" };
}

RePlayerAnimController::RePlayerAnimController(ModelAnimator* modelAnimator, RePlayerController* player)
	: m_pAnimator{ modelAnimator }, m_pPlayerController{ player }
{

}

void RePlayerAnimController::Initialize(const SceneContext& /*sceneContext*/)
{
}

void RePlayerAnimController::Update(const SceneContext& sceneContext)
{
	using namespace AnimNames;

	auto pInput = sceneContext.pInput;
	const auto& playerDesc = m_pPlayerController->GetDesc();
	const auto& currentAnim = m_pAnimator->GetClipName();

	bool isSprinting = pInput->IsActionTriggered(playerDesc->actionId_Sprint);
	bool isMoving = m_pPlayerController->IsMoving();
	bool isMovingBackward = pInput->IsActionTriggered(playerDesc->actionId_MoveBackward);
	bool isAiming = m_pPlayerController->IsAiming();
	m_pAnimator->SetPlayReversed(false);

	// inb4 yandere dev - no time to make a state machine :(
	if (isSprinting && isMoving)
	{
		if (currentAnim != AnimNames::Sprint)
			m_pAnimator->SetAnimation(AnimNames::Sprint);
	}
	else if (isMoving)
	{
		m_pAnimator->SetPlayReversed(isMovingBackward);
		if (currentAnim != AnimNames::Walk)
			m_pAnimator->SetAnimation(AnimNames::Walk);
	}
	else if (isAiming)
	{
		if (currentAnim != AnimNames::PistolIdle)
			m_pAnimator->SetAnimation(AnimNames::PistolIdle);
	}
	else
	{
		if (currentAnim != AnimNames::Idle) 
			m_pAnimator->SetAnimation(AnimNames::Idle);
	}
}

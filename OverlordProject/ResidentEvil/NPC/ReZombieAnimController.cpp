#include "stdafx.h"
#include "ReZombieAnimController.h"

#include "ReZombie.h"
namespace AnimNames {
	const std::wstring Idle{ L"Idle" };
	const std::wstring Walk{ L"Walk" };
	const std::wstring Dying{ L"Dying" };
	const std::wstring Attack{ L"Bite" };
}

ReZombieAnimController::ReZombieAnimController(ModelAnimator* modelAnimator, ReZombie* zombie)
	: m_pAnimator(modelAnimator)
	, m_pZombie(zombie)
{
	m_pAnimator->SetAnimation(AnimNames::Idle);
}

void ReZombieAnimController::Initialize(const SceneContext& /*sceneContext*/)
{

}

void ReZombieAnimController::Update(const SceneContext& /*sceneContext*/)
{
	//const auto& zombieDesc = m_pZombie->GetDesc();
	const auto& currentAnim = m_pAnimator->GetClipName();
	const auto& state = m_pZombie->GetState();

	switch (state)
	{
	case ZState::IDLE:
		if (currentAnim != AnimNames::Idle)
		{
			m_pAnimator->SetAnimation(AnimNames::Idle);
			m_pAnimator->Play();
		}
		break;

	case ZState::WALKING:
		if (currentAnim != AnimNames::Walk)
		{
			m_pAnimator->SetAnimation(AnimNames::Walk);
			m_pAnimator->Play();
		}
		break;
	}


}

void ReZombieAnimController::TriggerState(const ZState& state)
{
	const auto& currentAnim = m_pAnimator->GetClipName();

	switch (state)
	{
	case ZState::IDLE:
		if (currentAnim != AnimNames::Attack)
		{
			m_pAnimator->PlayOnce(AnimNames::Attack);
		}
		break;

	case ZState::DEAD:
		if (currentAnim != AnimNames::Dying)
		{
			m_pAnimator->PlayOnce(AnimNames::Dying);
		}
		break;

	}

}

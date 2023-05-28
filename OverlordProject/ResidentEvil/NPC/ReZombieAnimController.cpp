#include "stdafx.h"
#include "ReZombieAnimController.h"
#include "ReZombie.h"

using namespace ZAnimNames;

ReZombieAnimController::ReZombieAnimController(ModelAnimator* modelAnimator, ReZombie* zombie)
	: m_pAnimator(modelAnimator)
	, m_pZombie(zombie)
{
	m_pAnimator->SetAnimation(Idle);
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
		if (currentAnim != Idle)
		{
			m_pAnimator->SetAnimation(Idle);
			m_pAnimator->Play();
		}
		break;

	case ZState::WALKING:
		if (currentAnim != Walk)
		{
			m_pAnimator->SetAnimation(Walk);
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
	case ZState::ATTACKING:
		if (currentAnim != Attack)
		{
			m_pAnimator->PlayOnce(Attack, Idle);
		}
		break;
	case ZState::DEAD:
		if (currentAnim != Dying)
		{
			m_pAnimator->PlayOnce(Dying, Idle);
		}
		break;

	}

}

void ReZombieAnimController::TriggerState(const ZState& state, std::function<void()> onCompletion)
{
	const auto& currentAnim = m_pAnimator->GetClipName();

	switch (state)
	{
	case ZState::ATTACKING:
		if (currentAnim != Attack)
		{
			m_pAnimator->PlayOnce(Attack, Idle, onCompletion);
		}
		break;
	case ZState::DEAD:
		if (currentAnim != Dying)
		{
			m_pAnimator->PlayOnce(Dying, Idle, onCompletion);
		}
		break;
	}
}
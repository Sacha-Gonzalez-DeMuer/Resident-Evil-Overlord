#include "stdafx.h"
#include "ReZombie.h"
#include "FilePaths.h"


#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"
#include "Materials/Deferred/BasicMaterial_Deferred_Shadow.h"

#include "ResidentEvil/NPC/ReZombieAnimController.h"
#include "ResidentEvil/ReData.h"
#include "ResidentEvil/Player/RePlayerController.h"

#include "ResidentEvil/Components/HealthComponent.h"

ReZombie::ReZombie(const ReCharacterDesc& characterDesc)
	: m_CharacterDesc(characterDesc), m_Acceleration(0.5f)
{
}

void ReZombie::Initialize(const SceneContext&)
{
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	SetTag(L"Zombie");

	// Zombie Model
	ModelComponent* modelComponent{};
	m_pModelObject = AddChild(new GameObject());
	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(FilePath::ZOMBIE_DIFFUSE);
	modelComponent = m_pModelObject->AddComponent(new ModelComponent(FilePath::ZOMBIE_ANIMS_OVM));
	modelComponent->SetMaterial(pSkinnedMaterial);

	auto pModelTransform = m_pModelObject->GetTransform();
	pModelTransform->Scale(0.11f, 0.11f, 0.11f);
	pModelTransform->Translate(0.f, -m_CharacterDesc.controller.height / 2, 0.f);
	pModelTransform->Rotate(0.f, 180.f, 0.f);
	m_pAnimController = AddComponent(new ReZombieAnimController(modelComponent->GetAnimator(), this));

	auto pHealth = AddComponent(new HealthComponent());
	pHealth->OnDeath.AddFunction([this]() { Die(); });
	pHealth->OnTakeDamage.AddFunction([this]() {OnTakeDamage(); });


	auto bloodemitterObj = AddChild(new GameObject());
	ParticleEmitterSettings particleSettings{};
	particleSettings.minSize = .1f;
	particleSettings.maxSize = 2.f;
	particleSettings.maxEnergy = .3f;
	particleSettings.minEnergy = .1f;
	particleSettings.maxScale = 5.5f;
	particleSettings.maxEmitterRadius = 0.1f;
	particleSettings.minEmitterRadius = 0.05f;
	particleSettings.velocity = XMFLOAT3(-10.f, 10.f, 0.f);
	particleSettings.offset = XMFLOAT3(0.f, m_CharacterDesc.controller.height / 2, 0.f);
	m_pBloodEmitter = bloodemitterObj->AddComponent(new ParticleEmitterComponent(FilePath::BLOOD_PARTICLE, particleSettings, 200));

	OnPlayerDetected.AddFunction([this]()
		{
			SoundManager::Get()->GetSystem()->playSound(m_pDetectedSound, nullptr, false, &m_pZombieChannel);
		});
}

void ReZombie::Update(const SceneContext& sceneContext)
{
	if (m_Dead) return;
	if (sceneContext.pCamera->IsActive())
	{
		const float deltaTime{ sceneContext.pGameTime->GetElapsed() };
		if (m_CooldownTimer > 0)
		{
			m_CooldownTimer -= deltaTime;
			return;
		}

		// scan for player
		const auto& targetPos = m_pTarget->GetTransform()->GetPosition();
		const auto& myPos = GetTransform()->GetPosition();
		const auto& difference = XMVectorSubtract(XMLoadFloat3(&targetPos), XMLoadFloat3(&myPos));
		const auto& direction = XMVector3Normalize(difference);
		float distance = XMVectorGetX(XMVector3Length(difference));

		if (distance > 60.f) return;
		const auto& targetForward = m_pTarget->GetTransform()->GetForward();
		const auto& myForward = GetTransform()->GetForward();

		// ATTACK ANIM
		if (m_pAnimController->GetAnimName() == ZAnimNames::Attack)
		{
			m_State = ZState::ATTACKING;

			const auto& lockedPos = XMVectorScale(XMVector3Normalize(difference), m_CharacterDesc.attackDistance * .7f);
			XMFLOAT3 fdisplacement;
			XMStoreFloat3(&fdisplacement, lockedPos);
			m_pTarget->GetTransform()->Translate(myPos.x + fdisplacement.x, myPos.y + fdisplacement.y, myPos.z + fdisplacement.z);

			float zombieYaw = XMConvertToDegrees(atan2(XMVectorGetX(direction), XMVectorGetZ(direction)));
			float targetYaw = XMConvertToDegrees(atan2(XMVectorGetX(-direction), XMVectorGetZ(-direction)));

			m_pPlayer->SetYaw(targetYaw);
			GetTransform()->Rotate(0.f, zombieYaw, 0.f);
			return;
		}
		else if (m_pPlayer->IsAnimLocked())
		{
			m_CooldownTimer = 5.f;
			m_pPlayer->SetAnimLocked(false);
		};

		constexpr float aggroDist = 50.f;
		// if player is in range, move towards player
		if (distance < aggroDist && distance > m_CharacterDesc.attackDistance)
		{
			if (!m_DetectedPlayer) OnPlayerDetected.Invoke();

			m_State = ZState::WALKING;
			m_Speed += m_Acceleration;
			m_Speed = std::min(m_Speed, m_CharacterDesc.maxMoveSpeed);

			const auto& dispacement = XMVectorScale(direction, m_Speed * deltaTime);
			XMFLOAT3 fdisplacement;
			XMStoreFloat3(&fdisplacement, dispacement);
			m_pControllerComponent->Move(fdisplacement, 0.001f);


			// calculate angle between forward of target and forward of zombie
		
			const auto dot = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&targetForward), XMLoadFloat3(&myForward)));
			const auto angle = acosf(dot);

			constexpr float epsilon{ 2.0f };
			if (abs(angle) > epsilon)
			{
				//rotation_direction = object_forward_horizontal.cross(target_direction_normalized)
				const auto& targetDirection = XMVector3Normalize(XMLoadFloat3(&targetForward));
				const auto& myDirection = XMVector3Normalize(XMLoadFloat3(&myForward));
				const auto& rotationDirection = XMVector3Cross(myDirection, targetDirection);
				XMFLOAT3 frotationDirection;
				XMStoreFloat3(&frotationDirection, rotationDirection);

				float yaw_diff = angle * 180.0f / XM_PI;
				if (frotationDirection.y < 0)
					yaw_diff = -yaw_diff;

				m_TotalYaw += yaw_diff * m_CharacterDesc.rotationSpeed * deltaTime;

				GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);
			}
		}
		else if (distance <= m_CharacterDesc.attackDistance)
		{
			if(m_State != ZState::ATTACKING) Attack();
		}
		else
		{
			m_State = ZState::IDLE;
		}
	}
}

void ReZombie::SetTarget(RePlayerController* pPlayer)
{
	m_pPlayer = pPlayer;
	m_pTarget = pPlayer->GetTransform();
}

void ReZombie::Die()
{
	m_pAnimController->TriggerState(ZState::DEAD, [this]() {
		m_Dead = true; 
		m_pAnimController->Pause();
		});
}

void ReZombie::Reset()
{
	m_pAnimController->Reset();
	m_State = ZState::IDLE;
	m_pAnimController->TriggerState(ZState::IDLE);
	m_Dead = false;
	GetTransform()->Translate(m_CharacterDesc.spawnPosition);
}

void ReZombie::Attack()
{
	m_State = ZState::ATTACKING;
	m_pAnimController->TriggerState(ZState::ATTACKING);

	if (m_pTarget == m_pPlayer->GetTransform())
		m_pPlayer->GetAttacked();
}

void ReZombie::OnTakeDamage()
{
	if (m_Dead) return;
	m_pBloodEmitter->Emit(10);
}

void ReZombie::UpdateBehavior(const SceneContext& /*sceneContext*/)
{

}


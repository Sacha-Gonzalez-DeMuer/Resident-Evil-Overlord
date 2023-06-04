#pragma once
#include "ResidentEvil/ReData.h"
#include "Delegate.h"
class ReZombieAnimController;
class RePlayerController;
class ParticleEmitterComponent;
class HealthComponent;
class ReZombie final : public GameObject
{
public:
	ReZombie(const ReCharacterDesc& characterDesc);
	~ReZombie() override = default;
	ReZombie(const ReZombie& other) = delete;
	ReZombie(ReZombie&& other) noexcept = delete;
	ReZombie& operator=(const ReZombie& other) = delete;
	ReZombie& operator=(ReZombie&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
	void SetTarget(RePlayerController* pPlayer);
	void SetTarget(TransformComponent* pTarget) { m_pTarget = pTarget; }
	TransformComponent* GetTarget() const { return m_pTarget; }
	const ReCharacterDesc& GetDesc() const { return m_CharacterDesc; }
	ZState GetState() const { return m_State; }
	void Die();
	void Reset();
	void SetActive(bool active) { m_IsActive = active; }
	Delegate<> OnPlayerDetected;
private:
	// Components
	ParticleEmitterComponent* m_pBloodEmitter{};
	ReZombieAnimController* m_pAnimController{};
	ControllerComponent* m_pControllerComponent{};
	HealthComponent* m_pHealthComponent{};
	GameObject* m_pModelObject{};

	// Target info
	ReCharacterDesc m_CharacterDesc;
	RePlayerController* m_pPlayer{ nullptr };
	TransformComponent* m_pTarget{ nullptr };

	//movement
	float m_TotalYaw{};
	bool m_IsMoving{ false };
	float m_Speed{ 0.5f }, m_Acceleration;
	float m_RotationSpeed{ 0.5f };
	float m_Rotation{ 0.f };

	// Brain
	float m_CooldownTimer = { 5.f };
	bool m_Dead{ false };
	bool m_DetectedPlayer{ false };
	bool m_IsActive{ true };

	//Audio
	float m_MoanTimer{ 10.f };
	float m_MoanCooldown{ 5.f };
	float m_MinMoanCooldown{6.f};
	float m_MaxMoanCooldown{ 15.f };

	FMOD::System* m_pFMODSys{};
	FMOD::Channel* m_pZombieChannel{};
	FMOD::Sound* m_pIdleSound{};
	FMOD::Sound* m_pDetectedSound{};
	FMOD::Sound* m_pAggroSound{};

	ZState m_State{ ZState::IDLE };
	void UpdateBehavior(const SceneContext& sceneContext);
	void Attack();
	void OnTakeDamage();
};


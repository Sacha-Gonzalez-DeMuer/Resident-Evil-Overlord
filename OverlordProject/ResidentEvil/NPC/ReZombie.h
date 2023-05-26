#pragma once
#include "ResidentEvil/ReData.h"

class ReZombieAnimController;
class ReZombie final : public GameObject
{
public:
	ReZombie(const ReCharacterDesc& characterDesc);
	~ReZombie() override = default;
	ReZombie(const ReZombie& other) = delete;
	ReZombie(ReZombie&& other) noexcept = delete;
	ReZombie& operator=(const ReZombie& other) = delete;
	ReZombie& operator=(ReZombie&& other) noexcept = delete;


	enum class ZState
	{
		IDLE,
		WALKING,
		ATTACKING,
		DEAD
	};

	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;
	void SetTarget(TransformComponent* pTarget) { m_pTarget = pTarget; }
	TransformComponent* GetTarget() const { return m_pTarget; }
	const ReCharacterDesc& GetDesc() const { return m_CharacterDesc; }
	ZState GetState() const { return m_State; }
private:
	ReZombieAnimController* m_pAnimController{};
	ControllerComponent* m_pControllerComponent{};
	GameObject* m_pModelObject{};

	ReCharacterDesc m_CharacterDesc;

	float m_TotalYaw{};
	bool m_IsMoving{ false };

	TransformComponent* m_pTarget{ nullptr };
	float m_Speed{ 0.5f };
	float m_RotationSpeed{ 0.5f };
	float m_Rotation{ 0.f };

	ZState m_State{ ZState::IDLE };
	void UpdateBehavior(const SceneContext& sceneContext);
	
};


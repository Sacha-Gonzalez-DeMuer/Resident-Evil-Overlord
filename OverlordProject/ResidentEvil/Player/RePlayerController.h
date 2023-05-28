#pragma once
#include "ResidentEvil/ReData.h"

class RePlayerAnimController;
class ReInventory;
class RePlayerController : public GameObject
{
#pragma warning(disable : 4324) // https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4324?view=msvc-170 I couldn't find why this is happening 
public:
	RePlayerController(const ReCharacterDesc& characterDesc);
	~RePlayerController() override = default;

	RePlayerController(const RePlayerController& other) = delete;
	RePlayerController(RePlayerController&& other) noexcept = delete;
	RePlayerController& operator=(const RePlayerController& other) = delete;
	RePlayerController& operator=(RePlayerController&& other) noexcept = delete;

	//void DrawImGui();
	const XMFLOAT3& GetPosition() const { return m_pControllerComponent->GetPosition(); }
	const XMFLOAT3& GetTotalVelocity() const { return m_TotalVelocity; }
	ReCharacterDesc* GetDesc() { return &m_CharacterDesc; }
	ReInventory* GetInventory() const { return m_pInventory; }

	bool IsMoving() const { return m_IsMoving; }
	bool IsAiming() const { return m_IsAiming; }
	bool IsAnimLocked() const { return m_AnimationLocked; }
	void SetAnimLocked(bool locked) { m_AnimationLocked = locked; }
	void GetAttacked();
	ParticleEmitterComponent* GetEmitter() { return m_pBloodEmitter; };
	void SetYaw(const float yaw) { m_TotalYaw = yaw; }
protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	ReInventory* m_pInventory{};
	RePlayerAnimController* m_pAnimController{};
	ControllerComponent* m_pControllerComponent{};
	GameObject* m_pModelObject{};
	ParticleEmitterComponent* m_pBloodEmitter{};

	ReCharacterDesc m_CharacterDesc;
	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	XMVECTOR m_CamForward{};
	XMVECTOR m_CamRight{};
	float m_TotalYaw{};								//Total gamobject Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)


	float m_CamCleanTimer{ .1f };
	float m_CamCleanDelay{ .3f }; // amount of time before new camera transform is used for movement

	bool m_IsMoving{ false };
	bool m_IsAiming{ false };
	bool m_AnimationLocked{ false };
	//bool m_IsGettingBit{ false };

	void OnCamSwitch();
	void HandleInputActions(InputManager* input);
	void Interact();
	void Shoot();
	float m_GunDmg{ 25.f };

	GameObject* Raycast();
};
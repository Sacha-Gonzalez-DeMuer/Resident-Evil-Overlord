#pragma once
#include "RePlayerController.h"
struct ReCharacterDesc
{
	PxCapsuleControllerDesc controller{};

	XMFLOAT3 cameraOffset{ 0.f, controller.height * .5f, .0f };

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };
	float JumpSpeed{ 15.f };
	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };
	float rotationSpeed{ 60.f };
	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
	int actionId_Interact{ -1 };
	int actionId_Sprint{ -1 };

	ReCharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}
};

class RePlayerAnimController;
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
	const ReCharacterDesc& GetDesc() const { return m_CharacterDesc; }

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	RePlayerAnimController* m_pAnimController{};
	ControllerComponent* m_pControllerComponent{};
	GameObject* m_pModelObject{};

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


	void OnCamSwitch();
	void HandleInputActions(InputManager* input);
	void Interact();
};
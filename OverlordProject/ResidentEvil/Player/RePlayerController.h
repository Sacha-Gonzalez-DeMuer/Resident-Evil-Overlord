#pragma once
struct ReCharacterDesc
{
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
	XMFLOAT3 cameraOffset{ 0.f, controller.height * .5f, .0f };

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };

	float JumpSpeed{ 15.f };

	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };

	PxCapsuleControllerDesc controller{};

	bool isNavMeshController{ false };

	float rotationSpeed{ 60.f };

	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
};

class RePlayerController : public GameObject
{
public:
	RePlayerController(const ReCharacterDesc& characterDesc);
	~RePlayerController() override = default;

	RePlayerController(const RePlayerController& other) = delete;
	RePlayerController(RePlayerController&& other) noexcept = delete;
	RePlayerController& operator=(const RePlayerController& other) = delete;
	RePlayerController& operator=(RePlayerController&& other) noexcept = delete;

	//void DrawImGui();

protected:
	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

private:
	ControllerComponent* m_pControllerComponent{};

	ReCharacterDesc m_CharacterDesc;
	float m_TotalYaw{};								//Total gamobject Yaw(Y) rotation
	float m_MoveAcceleration{},						//Acceleration required to reach maxMoveVelocity after 1 second (maxMoveVelocity / moveAccelerationTime)
		m_FallAcceleration{},						//Acceleration required to reach maxFallVelocity after 1 second (maxFallVelocity / fallAccelerationTime)
		m_MoveSpeed{};								//MoveSpeed > Horizontal Velocity = MoveDirection * MoveVelocity (= TotalVelocity.xz)

	XMFLOAT3 m_TotalVelocity{};						//TotalVelocity with X/Z for Horizontal Movement AND Y for Vertical Movement (fall/jump)
	XMFLOAT3 m_CurrentDirection{};					//Current/Last Direction based on Camera forward/right (Stored for deacceleration)

	XMVECTOR m_CamForward{};
	XMVECTOR m_CamRight{};
	float m_CamCleanTimer{ .1f };
	float m_CamCleanDelay{ .3f }; // amount of time before new camera transform is used for movement
	void OnCamSwitch();
};
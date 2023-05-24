#pragma once
class FreeCamera final : public GameObject
{
public:
	FreeCamera() = default;
	~FreeCamera() override = default;
	FreeCamera(const FreeCamera& other) = delete;
	FreeCamera(FreeCamera&& other) noexcept = delete;
	FreeCamera& operator=(const FreeCamera& other) = delete;
	FreeCamera& operator=(FreeCamera&& other) noexcept = delete;

	void SetRotation(float pitch, float yaw);
	void Lock() { m_Locked = true; }
protected:
	void Initialize(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

private:
	bool m_Locked{ false };
	float m_TotalPitch{}, m_TotalYaw{};
	float m_MoveSpeed{10.f}, m_RotationSpeed{10.f}, m_SpeedMultiplier{10.f};
	CameraComponent* m_pCamera{};
};

#pragma once
class CubePrefab;
class ReCamera : public GameObject
{
public:
	ReCamera(const XMFLOAT3& position, bool targetable = true);
	virtual ~ReCamera() = default;
	ReCamera(const ReCamera& other) = delete;
	ReCamera(ReCamera&& other) noexcept = delete;
	ReCamera& operator=(const ReCamera& other) = delete;
	ReCamera& operator=(ReCamera&& other) noexcept = delete;

	CameraComponent* GetCamera() { return m_pCamera; }

	XMVECTOR GetLookAt() const { return m_pCamera->GetLookAt(); }
	XMVECTOR GetRight() const { return m_pCamera->GetRightVec(); }
	XMVECTOR GetUp() const { return m_pCamera->GetUpVec(); }

	float& GetPitch() { return m_TotalPitch; }
	float& GetYaw() { return m_TotalYaw; }
	float& GetRoll() { return m_TotalRoll; }
	float& GetFOV();
	XMFLOAT4& GetLightOrientation() { return m_LightOrientation; }
	XMFLOAT4& GetLightPosition()  { return m_LightPosition; }

	void SetLightOrientation(const XMFLOAT4& orientation) { m_LightOrientation = orientation; m_HasLight = true; }
	void SetLightPosition(const XMFLOAT4& position) { m_LightPosition = position; m_HasLight = true; }
	void SetHasLight(bool hasLight) { m_HasLight = hasLight; }
	bool HasLight() const { return m_HasLight; }

	int GetIdx() const { return m_idx; }
	void SetIdx(int idx) { m_idx = idx; }
	void ActivateCam();

	void Update(const SceneContext& sceneContext) override;
	void OnCameraSwitch();

private:
	CameraComponent* m_pCamera;

	int m_idx;
	float m_TotalPitch{}, m_TotalYaw{}, m_TotalRoll{};

	bool m_HasLight{ false }; // allow each camera to have it's own directional light 
	XMFLOAT4 m_LightOrientation{ 0.f, 0.f ,0.f, 1.0f };
	XMFLOAT4 m_LightPosition{ 0.f, 0.f, 0.f, 1.0f };

	void InitializeCamera(const XMFLOAT3& position, bool targetable = true);
};


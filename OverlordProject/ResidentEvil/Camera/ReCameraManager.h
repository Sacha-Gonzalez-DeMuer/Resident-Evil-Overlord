#pragma once
#include "Delegate.h"
class ReCamera;
class ReCameraManager final
{
public:
	static ReCameraManager& Get()
	{
		static ReCameraManager instance{};
		return instance;
	}

	~ReCameraManager() = default;
	ReCameraManager(const ReCameraManager& other) = delete;
	ReCameraManager(ReCameraManager&& other) noexcept = delete;
	ReCameraManager& operator=(const ReCameraManager& other) = delete;
	ReCameraManager& operator=(ReCameraManager&& other) noexcept = delete;
	int AddVolume(ReCamera* pVolume);
	ReCamera* GetActiveCamera() { return m_pActiveCamera; }
	int RemoveCamera(CameraComponent* pCamera);
	void SetActiveCamera(UINT index);
	void SetActiveCamera(ReCamera* pVolume);
	CameraComponent* GetCamera(UINT index) const { return m_pCameras[index]; }
	UINT GetCameraCount() const { return static_cast<UINT>(m_pCameras.size()); }

	void SetTarget(TransformComponent* pTarget) { m_pTarget = pTarget; }
	TransformComponent* GetTarget() const { return m_pTarget; }

	Delegate<> OnCameraSwitch;
private:
	ReCameraManager() = default;
	std::vector<ReCamera*> m_pCameraVolumes{};
	std::vector<CameraComponent*> m_pCameras{};
	//std::vector<bool> m_pActiveVolumes; //TODO: disable unnessecary volumes;

	ReCamera* m_pActiveCamera{ nullptr };
	TransformComponent* m_pTarget{ nullptr };
};
#pragma once
class CameraSwitch final : public GameObject
{
public:
	CameraSwitch(const XMFLOAT3& pos, const XMFLOAT3& size, bool debug = false);
	void SetTargets(const int cam0, const int cam1) { m_TargetCam0 = cam0; m_TargetCam1 = cam1; }

private:
	int m_TargetCam0, m_TargetCam1;

	void InitializeTriggerBox(const XMFLOAT3& pos, const XMFLOAT3& size);
	void OnTrigger(GameObject* trigger, GameObject* other, PxTriggerAction action);
};


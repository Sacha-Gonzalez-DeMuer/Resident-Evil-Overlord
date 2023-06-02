#pragma once
class CameraSwitch final : public GameObject
{
public:
	CameraSwitch(const XMFLOAT3& pos, const XMFLOAT3& size, bool debug = false);
	void SetTargets(const int cam0, const int cam1);
	void SetTarget(const int cam);

private:
	int m_TargetCam0, m_TargetCam1;
	bool m_SingleTarget;

	void InitializeTriggerBox(const XMFLOAT3& pos, const XMFLOAT3& size);
	void OnTrigger(GameObject* trigger, GameObject* other, PxTriggerAction action);
};


#pragma once
#include "ResidentEvil/World/IInteractable.h"

class ReDoor final : public GameObject
{
public:
	ReDoor(const XMFLOAT3& pos, const XMFLOAT3& size);
	~ReDoor() override = default;
	ReDoor(const ReDoor& other) = delete;
	ReDoor(ReDoor&& other) noexcept = delete;
	ReDoor& operator=(const ReDoor& other) = delete;
	ReDoor& operator=(ReDoor&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override;
	void Interact();
	void SetSceneToLoad(int idx){ m_sceneToLoadIdx = idx; }

private:
	void InitializeCollisionBox(const XMFLOAT3& pos, const XMFLOAT3& size);
	int m_sceneToLoadIdx{};
};

#pragma once
class ReInteractableObject : public GameObject
{
public:
	ReInteractableObject(const XMFLOAT3& pos, const XMFLOAT3& size);
	~ReInteractableObject() override = default;
	ReInteractableObject(const ReInteractableObject& other) = delete;
	ReInteractableObject(ReInteractableObject&& other) noexcept = delete;
	ReInteractableObject& operator=(const ReInteractableObject& other) = delete;
	ReInteractableObject& operator=(ReInteractableObject&& other) noexcept = delete;
	void Initialize(const SceneContext&) override {};
	virtual void Interact() = 0;


private:
	void InitializeCollisionBox(const XMFLOAT3& pos, const XMFLOAT3& size);
};
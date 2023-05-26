#pragma once

class InteractableComponent final : public BaseComponent
{
public:
	InteractableComponent() = default;
	~InteractableComponent() override = default;
	InteractableComponent(const InteractableComponent& other) = delete;
	InteractableComponent(InteractableComponent&& other) noexcept = delete;
	InteractableComponent& operator=(const InteractableComponent& other) = delete;
	InteractableComponent& operator=(InteractableComponent&& other) noexcept = delete;
	void Interact() { m_Interact(); }
	void SetInteractCallback(std::function<void()> callback) { m_Interact = callback; }

	void Initialize(const SceneContext&) override {};

private:
	std::function<void()> m_Interact{};
};



#pragma once
class IInteractable
{
public:
	IInteractable() = default;
	virtual ~IInteractable() = default;
	IInteractable(const IInteractable& other) = delete;
	IInteractable(IInteractable&& other) noexcept = delete;
	IInteractable& operator=(const IInteractable& other) = delete;
	IInteractable& operator=(IInteractable&& other) noexcept = delete;

	virtual void Interact() = 0;
};


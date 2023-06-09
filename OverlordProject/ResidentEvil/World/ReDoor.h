#pragma once
#include "ResidentEvil/World/ReInteractableObject.h"
#include "Delegate.h"

class ReDoor final : public ReInteractableObject
{
public:
	ReDoor(const XMFLOAT3& pos, const XMFLOAT3& size);
	~ReDoor() override = default;
	ReDoor(const ReDoor& other) = delete;
	ReDoor(ReDoor&& other) noexcept = delete;
	ReDoor& operator=(const ReDoor& other) = delete;
	ReDoor& operator=(ReDoor&& other) noexcept = delete;

	void Interact() override;

	Delegate<> OnInteract;
};

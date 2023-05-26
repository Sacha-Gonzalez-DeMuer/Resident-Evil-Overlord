#pragma once
#include "ResidentEvil/World/ReInteractableObject.h"

class ReInventory;
class ReGun final : public ReInteractableObject
{
public:
	ReGun(const XMFLOAT3& pos, const XMFLOAT3& size);
	~ReGun() override = default;
	ReGun(const ReGun& other) = delete;
	ReGun(ReGun&& other) noexcept = delete;
	ReGun& operator=(const ReGun& other) = delete;
	ReGun& operator=(ReGun&& other) noexcept = delete;

	void SetDestInventory(ReInventory* toInventory) { m_pDestInventory = toInventory;}
	void Interact() override;

private:
	ReInventory* m_pDestInventory{};
};


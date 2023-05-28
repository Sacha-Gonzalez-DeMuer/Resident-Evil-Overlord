#pragma once
#include <unordered_set>
#include "ResidentEvil/ReData.h"

class ReInventory final : public BaseComponent
{
public:
	ReInventory();
	~ReInventory() override = default;
	ReInventory(const ReInventory& other) = delete;
	ReInventory(ReInventory&& other) noexcept = delete;
	ReInventory& operator=(const ReInventory& other) = delete;
	ReInventory& operator=(ReInventory&& other) noexcept = delete;

	void Initialize(const SceneContext& /*sceneContext*/) override {};

	void EquipItem(const ReItem& item);
	bool HasGun() const;
	void AddItem(const ReItem& item);
	int GetItemAmount(const ReItem& item) const;

private:
	ReItem m_EquippedItem;
	std::vector<int> m_Items; // idx = item, value = amount, amount of guns = bullets
};


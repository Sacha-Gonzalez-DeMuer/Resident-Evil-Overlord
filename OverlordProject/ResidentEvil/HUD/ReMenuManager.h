#pragma once
#include "ResidentEvil/ReData.h"
#include <vector>

class ReMenu;
class ReMenuManager final : public GameObject
{
public:
	ReMenuManager() = default;
	~ReMenuManager() = default;
	ReMenuManager(const ReMenuManager& other) = delete;
	ReMenuManager(ReMenuManager&& other) noexcept = delete;
	ReMenuManager& operator=(const ReMenuManager& other) = delete;
	ReMenuManager& operator=(ReMenuManager&& other) noexcept = delete;

	void Initialize(const SceneContext&) override {};
	void SwitchMenu(ReMenuType type);
	void DisableMenus();
	void AddMenu(ReMenu* pMenu) { m_pMenus.push_back(pMenu); }


private:
	std::vector<ReMenu*> m_pMenus{};
	std::vector<ReMenu*> m_pLastActiveMenus{};
};


#pragma once
#include "ResidentEvil/ReData.h"
#include <vector>

class ReMenu;
class ReMenuManager final : public GameObject
{
public:
	static ReMenuManager& Get()
	{
		static ReMenuManager instance{};
		return instance;
	}

	~ReMenuManager() = default;
	ReMenuManager(const ReMenuManager& other) = delete;
	ReMenuManager(ReMenuManager&& other) noexcept = delete;
	ReMenuManager& operator=(const ReMenuManager& other) = delete;
	ReMenuManager& operator=(ReMenuManager&& other) noexcept = delete;

	void SwitchCanvas(ReMenuType type);

private:
	ReMenuManager() = default;
	std::vector<ReMenu*> m_pMenus{};
	std::vector<ReMenu*> m_pLastActiveMenus{};
};


#include "stdafx.h"
#include "ReMenuManager.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"




void ReMenuManager::SwitchMenu(ReMenuType type)
{
	for (auto pLastActiveMenu : m_pLastActiveMenus)
	{
		pLastActiveMenu->SetActive(false);
	}
	m_pLastActiveMenus.clear();

	for (auto pMenu : m_pMenus)
	{
		if (pMenu->GetType() == type)
		{
			pMenu->SetActive(true);
			m_pLastActiveMenus.push_back(pMenu);
		} else { Logger::LogWarning(L"ReMenuManager::SwitchMenu > Menu type not found!"); }
	}
}

void ReMenuManager::DisableMenus()
{
	for (auto pMenu : m_pMenus)
	{
		pMenu->SetActive(false);
	}
	m_pLastActiveMenus.clear();
}

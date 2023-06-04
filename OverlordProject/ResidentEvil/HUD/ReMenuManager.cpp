#include "stdafx.h"
#include "ReMenuManager.h"
#include "ResidentEvil/HUD/Menus/ReMenu.h"




void ReMenuManager::SwitchMenu(ReMenuType type)
{
	for (auto pMenu : m_pMenus)
	{
		if (pMenu->IsActive() && pMenu->GetType() != type)
		{
			pMenu->SetActive(false);
		}

		if (pMenu->GetType() == type)
		{
			pMenu->SetActive(true);
		} 
		else 
		{ Logger::LogWarning(L"ReMenuManager::SwitchMenu > Menu type not found!"); }
	}
}

void ReMenuManager::DisableMenus()
{
	for (auto pMenu : m_pMenus)
	{
		pMenu->SetActive(false);
	}
}

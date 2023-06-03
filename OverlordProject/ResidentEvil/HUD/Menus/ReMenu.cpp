#include "stdafx.h"
#include "ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"

ReMenu::ReMenu(ReMenuType type, const std::wstring& backgroundImg)
	: m_Type(type)
{
	if(backgroundImg != L"") m_pBackgroundImg = AddComponent(new SpriteComponent(backgroundImg, {0.5f,0.5f}, {1.f,1.f,1.f,.5f}));
}

void ReMenu::SetActive(bool isActive)
{
	m_IsActive = isActive;

	if(m_pBackgroundImg) m_pBackgroundImg->SetActive(m_IsActive);
	for (auto pBtn : m_pButtons)
	{
		pBtn->SetActive(m_IsActive);
	}
}

void ReMenu::AddButton(ReButton* pButton)
{
	m_pButtons.push_back(pButton);
	AddChild(pButton);
}

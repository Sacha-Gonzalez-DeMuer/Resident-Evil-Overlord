#include "stdafx.h"
#include "ReMenu.h"
#include "ResidentEvil/HUD/ReButton.h"
#include "FilePaths.h"
ReMenu::ReMenu(ReMenuType type)
	: m_Type(type)
{
}

void ReMenu::Initialize(const SceneContext&)
{
	m_pSoundSystem = SoundManager::Get()->GetSystem();
	const auto& hoverSoundPath = ContentManager::GetFullAssetPath(FilePath::BUTTON_HOVER_AUDIO);
	const auto& clickSoundPath = ContentManager::GetFullAssetPath(FilePath::BUTTON_CLICK_AUDIO);
	m_pSoundSystem->createStream(hoverSoundPath.string().c_str(), FMOD_LOOP_OFF | FMOD_2D, nullptr, &m_pHoveredSound);
	m_pSoundSystem->createStream(clickSoundPath.string().c_str(), FMOD_LOOP_OFF | FMOD_2D, nullptr, &m_pClickedSound);
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
	pButton->m_OnHover.AddFunction([this]() { PlayHover(); });
	pButton->m_OnClick.AddFunction([this]() { PlayClick(); });
	m_pButtons.push_back(pButton);
	AddChild(pButton);
}

void ReMenu::PlayHover()
{
	m_pSoundSystem->playSound(m_pHoveredSound, nullptr, false, &m_pMenuChannel);
}

void ReMenu::PlayClick()
{
	m_pSoundSystem->playSound(m_pClickedSound, nullptr, false, &m_pMenuChannel);
}

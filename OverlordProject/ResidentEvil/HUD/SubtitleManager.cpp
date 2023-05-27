#include "stdafx.h"
#include "SubtitleManager.h"
#include "ResidentEvil/HUD/SubtitleController.h"

void SubtitleManager::Initialize(const SceneContext& /*sceneContext*/)
{}


void SubtitleManager::SetSubtitle(const Subtitle& subtitle)
{
	m_pSubtitleController->SetSubtitle(subtitle);
}

void SubtitleManager::CloseSubtitle()
{
	m_pSubtitleController->CloseSubtitle();
}

bool SubtitleManager::IsDisplaying() const
{
	return m_pSubtitleController->IsDisplaying();
}

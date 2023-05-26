#include "stdafx.h"
#include "ReClock.h"
#include "ResidentEvil/HUD/SubtitleManager.h"

ReClock::ReClock(const XMFLOAT3& pos, const XMFLOAT3& size)
	: ReInteractableObject(pos, size)
{
}

void ReClock::Interact()
{
	Logger::LogWarning(L"ReClock::Interact()");
	SubtitleManager::Get()->SetSubtitle(m_Subtitle);
}

#include "stdafx.h"
#include "SubtitleController.h"

SubtitleController::SubtitleController()
{
}

void SubtitleController::Initialize(const SceneContext&)
{
	m_SubtitlePos = { 0, 0 };
	m_SubtitleColor = { 1, 1, 1, 1 };
}

void SubtitleController::Update(const SceneContext&)
{
	if (m_Displaying)
	{
		Logger::LogDebug(L"SubtitleManager::Draw()");
		TextRenderer::Get()->DrawText(m_CurrentSubtitle.font, StringUtil::utf8_decode(m_CurrentSubtitle.text), m_SubtitlePos, m_SubtitleColor);
	}
}

void SubtitleController::SetSubtitle(const Subtitle& subtitle)
{
	m_CurrentSubtitle = subtitle;
	m_Displaying = true;
}

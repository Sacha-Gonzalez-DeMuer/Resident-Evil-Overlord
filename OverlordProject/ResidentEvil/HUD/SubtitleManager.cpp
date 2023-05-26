#include "stdafx.h"
#include "SubtitleManager.h"
#include "ResidentEvil/ReData.h"

void SubtitleManager::Initialize(const SceneContext& /*sceneContext*/)
{
	m_SubtitlePos = { 0, 0 };
	m_SubtitleColor = { 1, 1, 1, 1 };
}

void SubtitleManager::Update(const SceneContext& /*sceneContext*/)
{
	if (m_Displaying)
	{
		TextRenderer::Get()->DrawText(m_CurrentSubtitle.font, StringUtil::utf8_decode(m_CurrentSubtitle.text), m_SubtitlePos, m_SubtitleColor);

		//if (sceneContext.pInput->IsActionTriggered(m_pCharacterDesc->actionId_Interact))
		//{
		//	m_Displaying = false;
		//}
	}
}

void SubtitleManager::SetSubtitle(const Subtitle& subtitle)
{
	m_CurrentSubtitle = subtitle; 
	m_Displaying = true;
}

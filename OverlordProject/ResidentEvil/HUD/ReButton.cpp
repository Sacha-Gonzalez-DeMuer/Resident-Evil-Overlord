#include "stdafx.h"
#include "ReButton.h"

ReButton::ReButton(const std::wstring& backgroundImg)
{
	m_pBackgroundImg = AddComponent(new SpriteComponent(backgroundImg, { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
}

void ReButton::Initialize(const SceneContext&)
{
}


void ReButton::SetText(const std::wstring& )
{
}

void ReButton::SetActive(const bool active)
{
	m_pBackgroundImg->SetActive(active);

	//todo: disable text
}

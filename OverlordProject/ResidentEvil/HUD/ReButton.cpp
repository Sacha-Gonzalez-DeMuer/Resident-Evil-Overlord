#include "stdafx.h"
#include "ReButton.h"
#include "FilePaths.h"


ReButton::ReButton(SpriteFont* font)
	: m_pFont{font}
	, m_pBackgroundImg{nullptr}
{
	if (!m_pFont) m_pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);
}

ReButton::ReButton(const std::wstring& backgroundImg)
	: m_pFont{ nullptr }
	, m_pBackgroundImg{ nullptr }
{
	m_pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);
	if (backgroundImg != L"")m_pBackgroundImg = AddComponent(new SpriteComponent(backgroundImg, { 0.5f,0.5f }, { 1.f,1.f,1.f,.5f }));
}

ReButton::ReButton(SpriteFont* font, const std::wstring& backgroundImg)
: m_pFont(font), m_pBackgroundImg{nullptr}
{
	if (!m_pFont) m_pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);
	if(backgroundImg != L"")m_pBackgroundImg = AddComponent(new SpriteComponent(backgroundImg, {0.5f,0.5f}, {1.f,1.f,1.f,.5f}));
}

void ReButton::Initialize(const SceneContext&)
{
	const auto& currentPos = GetTransform()->GetPosition();
	const auto& fontDesc = m_pFont->GetDescription();
	GetTransform()->Translate(currentPos.x - fontDesc.textureWidth * 2, currentPos.y - fontDesc.textureHeight * 2, 0.9f);
}

void ReButton::Draw(const SceneContext& /*sceneContext*/)
{
	if(m_Text.empty()) return;

	auto pTransform = GetTransform();
	const auto& pos = pTransform->GetPosition();

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), { pos.x, pos.y });
}

void ReButton::Update(const SceneContext& sceneContext)
{
	const auto mousePos = sceneContext.pInput->GetMousePosition();
	const auto btnPos = GetTransform()->GetPosition();
	const auto btnScale = GetTransform()->GetScale();



	const auto btnWidth = btnScale.x * m_pFont->GetDescription().textureWidth * 2;
	const auto btnHeight = btnScale.y * m_pFont->GetDescription().textureHeight * 2;

	// print everything
	//std::cout << "mousePos: " << mousePos.x << ", " << mousePos.y << std::endl;
	//std::cout << "btnPos: " << btnPos.x << ", " << btnPos.y << std::endl;
	//std::cout << "btnWidth: " << btnWidth << ", " << btnWidth << std::endl;
	//std::cout << "btnHeight: " << btnHeight  << ", " << btnHeight << std::endl;
	// check if mouse is clicked

	// check if mousepos in button
	if (mousePos.x > btnPos.x 
		&& mousePos.x < btnPos.x + btnWidth
		&& mousePos.y > btnPos.y
		&& mousePos.y < btnPos.y + btnHeight)
	{
		if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
		{
			m_OnClick();
		}
	}

}


void ReButton::SetText(const std::string& text)
{
	m_Text = text;
}

void ReButton::SetActive(const bool /*active*/)
{
	//m_pBackgroundImg->SetActive(active);

	//todo: disable text 
}

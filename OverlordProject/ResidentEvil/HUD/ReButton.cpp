#include "stdafx.h"
#include "ReButton.h"
#include "FilePaths.h"


ReButton::ReButton(const XMFLOAT2& pos, const XMFLOAT2& size, SpriteFont* font)
	: m_pFont{font}
	, m_Position{pos}
{
	if (!m_pFont) m_pFont = ContentManager::Load<SpriteFont>(FilePath::DEFAULT_FONT);
	m_Width = size.x;
	m_Height = size.y;
	GetTransform()->Translate(pos.x - m_Width * .5f, pos.y - m_Height*.5f, 0);
}

void ReButton::Initialize(const SceneContext&)
{
}

void ReButton::Draw(const SceneContext& /*sceneContext*/)
{
	if(m_Text.empty()) return;

	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), { m_Position.x, m_Position.y });
}

void ReButton::Update(const SceneContext& sceneContext)
{
	const auto mousePos = sceneContext.pInput->GetMousePosition();

	// print everything
	std::cout << "mouse pos: " << mousePos.x << ", " << mousePos.y << std::endl;
	std::cout << "button pos: " << m_Position.x << ", " << m_Position.y << std::endl;
	std::cout << "button width: " << m_Width << std::endl;
	std::cout << "button height: " << m_Height << std::endl;
	std::cout << "////////////////////////////////////////////" << std::endl;


	// check if mousepos in button
	if (mousePos.x > m_Position.x
		&& mousePos.x < m_Position.x + m_Width
		&& mousePos.y > m_Position.y
		&& mousePos.y < m_Position.y + m_Height)
	{
		if (!m_IsHovered)
		{
			m_IsHovered = true;
			m_OnHover.Invoke();
		}
		if (InputManager::IsMouseButton(InputState::pressed, VK_LBUTTON))
		{
			m_OnClick.Invoke();
		}
	}
	else m_IsHovered = false;

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

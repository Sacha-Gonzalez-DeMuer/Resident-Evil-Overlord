#pragma once
#include "Delegate.h"
class ReButton  : public GameObject
{
public:
	ReButton(const XMFLOAT2& pos, const XMFLOAT2& size, SpriteFont* font = nullptr);
	~ReButton() override = default;
	ReButton(const ReButton& other) = delete;
	ReButton(ReButton&& other) noexcept = delete;
	ReButton& operator=(const ReButton& other) = delete;
	ReButton& operator=(ReButton&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;
	void Draw(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

	void SetText(const std::string& text);
	void AddOnClick(std::function<void()> onClick) 
	{ m_OnClick.AddFunction(onClick); }
	void SetActive(const bool active);

	// on click for menu sys
	Delegate<> m_OnClick{};
	Delegate<> m_OnHover{};
private:
	SpriteFont* m_pFont{};
	std::string m_Text{};
	bool m_IsHovered{ false };
	bool m_IsActive{ true };

	XMFLOAT2 m_Position{};
	XMFLOAT2 m_HoverOffset{ 5.f, 5.f };
	float m_Width{};
	float m_Height{};
};


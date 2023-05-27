#pragma once
class ReButton final : GameObject
{
public:
	ReButton(const std::wstring& backgroundImg);
	~ReButton() override = default;
	ReButton(const ReButton& other) = delete;
	ReButton(ReButton&& other) noexcept = delete;
	ReButton& operator=(const ReButton& other) = delete;
	ReButton& operator=(ReButton&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;

	void SetText(const std::wstring& text);
	void SetOnClick(std::function<void()> onClick) { m_OnClick = onClick; }

private:
	std::function<void()> m_OnClick{};
	std::wstring m_Text{};
	bool m_IsClicked{ false };
	

	//todo
	//bool m_IsReleased{ false };
	//bool m_IsHovered{ false };
	//bool m_IsPressed{ false };
};


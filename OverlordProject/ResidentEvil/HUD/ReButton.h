#pragma once
class ReButton  : public GameObject
{
public:
	ReButton(SpriteFont* font = nullptr);
	ReButton(const std::wstring& backgroundImg);
	ReButton(SpriteFont* font, const std::wstring& backgroundImg);
	~ReButton() override = default;
	ReButton(const ReButton& other) = delete;
	ReButton(ReButton&& other) noexcept = delete;
	ReButton& operator=(const ReButton& other) = delete;
	ReButton& operator=(ReButton&& other) noexcept = delete;
	void Initialize(const SceneContext& sceneContext) override;
	void Draw(const SceneContext& sceneContext) override;
	void Update(const SceneContext& sceneContext) override;

	void SetText(const std::string& text);
	void SetOnClick(std::function<void()> onClick) { m_OnClick = onClick; }
	void SetActive(const bool active);

	
private:
	SpriteFont* m_pFont;
	SpriteComponent* m_pBackgroundImg;
	std::function<void()> m_OnClick{};
	std::string m_Text{};
	bool m_IsClicked{ false };
	

	//todo
	//bool m_IsReleased{ false };
	//bool m_IsHovered{ false };
	//bool m_IsPressed{ false };
};


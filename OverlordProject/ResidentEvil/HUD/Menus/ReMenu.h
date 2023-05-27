#pragma once
#include "ResidentEvil/ReData.h"

class ReButton;
class ReMenu : public GameObject
{
public:
	ReMenu(ReMenuType type, const std::wstring& backgroundImg);
	~ReMenu() override = default;
	ReMenu(const ReMenu& other) = delete;
	ReMenu(ReMenu&& other) noexcept = delete;
	ReMenu& operator=(const ReMenu& other) = delete;
	ReMenu& operator=(ReMenu&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext) override {};
	void SetActive(bool isActive);
	bool IsActive() const { return m_IsActive; }
	ReMenuType GetType() const { return m_Type; }
	void AddButton(ReButton* pButton) { m_pButtons.push_back(pButton); }

private:
	ReMenuType m_Type{};
	bool m_IsActive{ false };
	std::vector<ReButton*> m_pButtons{};
	SpriteComponent* m_pBackgroundImg{};

};


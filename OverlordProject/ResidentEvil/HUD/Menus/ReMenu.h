#pragma once
#include "ResidentEvil/ReData.h"

class ReButton;
class ReMenu : public GameObject
{
public:
	ReMenu(ReMenuType type, const std::wstring& backgroundImg = L"");
	~ReMenu() override = default;
	ReMenu(const ReMenu& other) = delete;
	ReMenu(ReMenu&& other) noexcept = delete;
	ReMenu& operator=(const ReMenu& other) = delete;
	ReMenu& operator=(ReMenu&& other) noexcept = delete;

	void Initialize(const SceneContext&) override {};
	void SetActive(bool isActive);
	bool IsActive() const { return m_IsActive; }
	ReMenuType GetType() const { return m_Type; }
	void AddButton(ReButton* pButton);

private:
	ReMenuType m_Type{};
	bool m_IsActive{ true };
	std::vector<ReButton*> m_pButtons{};
	SpriteComponent* m_pBackgroundImg{};
};


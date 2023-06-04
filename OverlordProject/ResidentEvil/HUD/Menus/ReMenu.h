#pragma once
#include "ResidentEvil/ReData.h"

class ReButton;
class ReMenu : public GameObject
{
public:
	ReMenu(ReMenuType type);
	~ReMenu() override = default;
	ReMenu(const ReMenu& other) = delete;
	ReMenu(ReMenu&& other) noexcept = delete;
	ReMenu& operator=(const ReMenu& other) = delete;
	ReMenu& operator=(ReMenu&& other) noexcept = delete;

	void Initialize(const SceneContext&) override;
	void SetActive(bool isActive);
	bool IsActive() const { return m_IsActive; }
	ReMenuType GetType() const { return m_Type; }
	void AddButton(ReButton* pButton);

private:
	ReMenuType m_Type{};
	bool m_IsActive{ true };
	std::vector<ReButton*> m_pButtons{};
	SpriteComponent* m_pBackgroundImg{};

	FMOD::System* m_pSoundSystem{};
	FMOD::Channel* m_pMenuChannel{};
	FMOD::Sound* m_pHoveredSound{};
	FMOD::Sound* m_pClickedSound{};

	void PlayHover();
	void PlayClick();
};


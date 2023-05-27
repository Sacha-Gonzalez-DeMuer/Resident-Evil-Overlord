#pragma once
class ThunderController final : public GameObject
{
public:
	ThunderController() = default;
	~ThunderController() override = default;
	ThunderController(const ThunderController& other) = delete;
	ThunderController(ThunderController&& other) noexcept = delete;
	ThunderController& operator=(const ThunderController& other) = delete;
	ThunderController& operator=(ThunderController&& other) noexcept = delete;

	void Initialize(const SceneContext&) override {};
	void Update(const SceneContext&) override;

private:
	// times in in milliseconds
	float m_FlashDuration{ 300 };
	float m_MinDelay{ 100 };
	float m_MaxDelay{ 5000 };
	float m_CurrentFlashDuration{ 0 };
	float m_CurrentDelay{ 0 };
};
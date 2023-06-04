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
	void Disable() {m_Enabled = false;}
	void Enable() {m_Enabled = true;}

	void SetMaxDelay(float delay) { m_MaxDelay = delay; }


private:
	bool m_Enabled{true};
	float m_FlashDuration{ 0.1f };

	float m_MinFlashDuration{ 0.01f };
	float m_MaxFlashDuration{ 0.4f };

	float m_MinDelay{ .1f };
	float m_MaxDelay{ 10.f };

	float m_MinIntensity{ .3f };
	float m_MaxIntensity{ 1.f };

	float m_CurrentFlashDuration{ .1f };
	float m_CurrentDelay{ .1f };
	float m_DelayTimer{ 0 };

	bool m_isLightOn{ true };
};
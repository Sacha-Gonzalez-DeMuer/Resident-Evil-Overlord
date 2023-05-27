#pragma once
#include "ResidentEvil/ReData.h"

class SubtitleController final : public GameObject
{
public:
	SubtitleController();
	~SubtitleController() override = default;
	SubtitleController(const SubtitleController& other) = delete;
	SubtitleController(SubtitleController&& other) noexcept = delete;
	SubtitleController& operator=(const SubtitleController& other) = delete;
	SubtitleController& operator=(SubtitleController&& other) noexcept = delete;

	void Initialize(const SceneContext&) override;
	void Update(const SceneContext&) override;

	void SetSubtitle(const Subtitle& subtitle);
	void CloseSubtitle() { m_Displaying = false; }
	bool IsDisplaying() const { return m_Displaying; }

	private:
		bool m_Displaying{};
		Subtitle m_CurrentSubtitle{};
		XMFLOAT2 m_SubtitlePos{};
		XMFLOAT4 m_SubtitleColor{};


};


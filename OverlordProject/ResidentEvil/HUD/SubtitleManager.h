#pragma once
#include "ResidentEvil/ReData.h"

struct ReCharacterDesc;
class SubtitleManager final 
{
public:
	static SubtitleManager& Get()
	{
		static SubtitleManager instance{};
		return instance;
	}

	~SubtitleManager() = default;
	SubtitleManager(const SubtitleManager& other) = delete;
	SubtitleManager(SubtitleManager&& other) noexcept = delete;
	SubtitleManager& operator=(const SubtitleManager& other) = delete;
	SubtitleManager& operator=(SubtitleManager&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext);
	void Update(const SceneContext& sceneContext);

	void SetSubtitle(const Subtitle& subtitle);
	void CloseSubtitle() { m_Displaying = false; }
	bool IsDisplaying() const { return m_Displaying; }

private:
	SubtitleManager() = default;
	bool m_Displaying{};
	Subtitle m_CurrentSubtitle{};
	XMFLOAT2 m_SubtitlePos{};
	XMFLOAT4 m_SubtitleColor{};

};
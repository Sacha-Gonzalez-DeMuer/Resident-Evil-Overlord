#pragma once
#include "ResidentEvil/ReData.h"

struct ReCharacterDesc;
class SubtitleManager final : public Singleton<SubtitleManager>
{
public:
	SubtitleManager() = default;
	~SubtitleManager() = default;
	SubtitleManager(const SubtitleManager& other) = delete;
	SubtitleManager(SubtitleManager&& other) noexcept = delete;
	SubtitleManager& operator=(const SubtitleManager& other) = delete;
	SubtitleManager& operator=(SubtitleManager&& other) noexcept = delete;

	void Initialize(const SceneContext& sceneContext);
	void Update(const SceneContext& sceneContext);

	void SetCharacterDesc(ReCharacterDesc* pCharacterDesc) { m_pCharacterDesc = pCharacterDesc; }
	void SetSubtitle(const Subtitle& subtitle);

private:
	bool m_Displaying{};
	Subtitle m_CurrentSubtitle{};
	XMFLOAT2 m_SubtitlePos{};
	XMFLOAT4 m_SubtitleColor{};

	ReCharacterDesc* m_pCharacterDesc{};
};
#pragma once
#include "ResidentEvil/ReData.h"

struct ReCharacterDesc;
class SubtitleController;
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
	void SetController(SubtitleController* controller) { m_pSubtitleController = controller; }

	void SetSubtitle(const Subtitle& subtitle);
	void CloseSubtitle();
	bool IsDisplaying() const;

private:
	SubtitleManager() = default;
	SubtitleController* m_pSubtitleController{};
};
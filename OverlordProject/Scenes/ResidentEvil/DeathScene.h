#pragma once
#include "Scenes/ResidentEvil/ReScene.h"

class ReMenuManager;
class DeathScene final : public ReScene
{
public:
	DeathScene(void) : ReScene(L"DeathScene") {};
	virtual ~DeathScene(void) {};
	void Initialize() override;

	void Start() override;
	void Reset() override;

private:
	ReMenuManager* m_pMenuManager{};
	SpriteComponent* m_pDeathScreen{};

	FMOD::Channel* m_pDeathChannel;
	FMOD::Sound* m_pDeathMusic;

	void RestartGame();
};


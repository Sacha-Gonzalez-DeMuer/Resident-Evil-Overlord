#pragma once
#include "Scenes/ResidentEvil/ReScene.h"

class ReMenuManager;
class ReButton;
class ReMenu;
class MainMenuScene final : public ReScene
{
public:
	MainMenuScene(void) : ReScene(L"MainMenu") {};
	virtual ~MainMenuScene(void) {};
	void Initialize() override;
	void OnGUI() override;
private:
	ReMenuManager* m_pMenuManager{};

	std::vector<ReButton*> m_pButtons{};
	std::vector<ReMenu*> m_pMenus{};
	SpriteComponent* m_pBackground{};
	SpriteComponent* m_pControlsImg{};
	FMOD::System* m_pFMODSys{};
	FMOD::Channel* m_pMenuChannel{};
	FMOD::Sound* m_pEVILSound{};
	FMOD::Sound* m_pStandBySound{};

	bool m_Wait;

	void Start() override {};
	void Reset() override {};
	void StartGame();
};


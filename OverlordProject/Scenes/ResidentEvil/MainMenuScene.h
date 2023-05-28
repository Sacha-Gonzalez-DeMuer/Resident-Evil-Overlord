#pragma once

class ReMenuManager;
class ReButton;
class ReMenu;
class MainMenuScene final : public GameScene
{
public:
	MainMenuScene(void) : GameScene(L"MainMenu") {};
	virtual ~MainMenuScene(void) {};
	void Initialize() override;
	void OnGUI() override;

private:
	ReMenuManager* m_pMenuManager{};

	std::vector<ReButton*> m_pButtons{};
	std::vector<ReMenu*> m_pMenus{};

	void StartGame();
	void Exit();
};


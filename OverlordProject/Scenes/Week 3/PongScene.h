#pragma once
#include <memory>

class Player;
class SpherePrefab;
class PongGameSettings;

class PongScene : public GameScene
{
public:
	PongScene();
	void Initialize() override;

	~PongScene() override = default;

	PongScene(const PongScene& other) = delete;
	PongScene(PongScene&& other) noexcept = delete;
	PongScene& operator=(const PongScene& other) = delete;
	PongScene& operator=(PongScene&& other) noexcept = delete;

	

protected:

private:
	std::unique_ptr<PongGameSettings> m_pGameSettings;
	std::vector<Player*> m_pPlayers;

	SpherePrefab* m_pPongBall;

	void HandleInput();
	void Update() override;
};


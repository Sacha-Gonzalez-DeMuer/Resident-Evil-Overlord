#pragma once
#include "Prefabs/PongGameSettings.h"

// pong player
class Player final : public GameObject
{
public:
	Player(const PongGameSettings& settings, const SceneContext& sceneContext, bool isLeftPlayer);

	void Initialize(const PongGameSettings& settings, bool isLeftPlayer);
	void Update(const SceneContext& sceneContext) override;
	
private:
	void HandleInput(float deltaTime);
	InputManager* m_pInput;
	GameObject* m_pPeddle;

	PongGameSettings::InputIds m_InputUp;
	PongGameSettings::InputIds m_InputDown;

	float m_moveSpeed;
	float m_maxHeight;
	float m_maxWidth;
};


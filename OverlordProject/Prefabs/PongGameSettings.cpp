#include "stdafx.h"
#include "PongGameSettings.h"
#include "Player.h"

PongGameSettings::PongGameSettings()
	: m_leftPosition{ XMFLOAT3(-30, 0, 0) }
{
}

void PongGameSettings::Initialize(vector<Player*>& playersContainer, const SceneContext& sceneContext, float rightPos)
{
	InitializeInput(sceneContext);
	m_leftPosition.x = rightPos;

	// initialize players
	playersContainer.clear();
	playersContainer.push_back( new Player(*this, sceneContext, true));
	playersContainer.push_back( new Player(*this, sceneContext, false));
}

void PongGameSettings::InitializeInput(const SceneContext& sceneContext)
{
	sceneContext.pInput->AddInputAction(InputAction
		{
			lInput_Down, InputState::down, VK_DOWN
		});
	sceneContext.pInput->AddInputAction(InputAction
		{
			lInput_Up, InputState::down, VK_UP
		});

	sceneContext.pInput->AddInputAction(InputAction
		{
			rInput_Down, InputState::down, 'J'
		});
	sceneContext.pInput->AddInputAction(InputAction
		{
			rInput_Up, InputState::down, 'U'
		});
	sceneContext.pInput->AddInputAction(InputAction
		{
			StartGame, InputState::pressed, VK_SPACE
		});

}

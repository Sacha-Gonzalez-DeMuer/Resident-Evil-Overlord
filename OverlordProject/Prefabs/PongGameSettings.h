#pragma once
#include <vector>
#include <memory>
using namespace std;

class Player;
class PongGameSettings final
{
public:
	PongGameSettings();


	void Initialize(vector<Player*>& playersContainer, const SceneContext& sceneContext, float leftPos);

	XMFLOAT3 GetLeftPosition() const { return m_leftPosition; };
	XMFLOAT3 GetRightPosition() const { return {-m_leftPosition.x, m_leftPosition.y, m_leftPosition.z}; };
	enum InputIds
	{
		lInput_Up,
		lInput_Down,
		rInput_Up,
		rInput_Down,
		StartGame
	};


private:
	// play field settings
	XMFLOAT3 m_leftPosition; // right position = -leftPosition

	void InitializeInput(const SceneContext& sceneContext);
};

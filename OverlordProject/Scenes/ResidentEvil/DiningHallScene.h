#pragma once
#include "Prefabs/Character.h"
class RePlayerController;
class CameraSwitch;
class ReDoor;
class DiningHallScene final : public GameScene
{
	public:
	DiningHallScene(void);
	virtual ~DiningHallScene(void);

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void PostDraw() override;

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		Interact,
		CharacterSprint,
	};

	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};

	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};

	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
};

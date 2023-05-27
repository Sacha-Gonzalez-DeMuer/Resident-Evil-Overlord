#pragma once
#include "Prefabs/Character.h"
class RePlayerController;
class CameraSwitch;
class ReDoor;
class ReClock;
class ReGun;
class ReZombie;
class ParticleEmitterComponent;
class DiningHallScene final : public GameScene
{
	public:
	DiningHallScene(void);
	virtual ~DiningHallScene(void);

	void Initialize() override;
	void Update() override;
	void Draw() override;
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
		CharacterAim,
		CharacterAttack
	};

	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};

	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	ReZombie* m_pZombie{};
	ReClock* m_pClock{};
	ReGun* m_pGun{};
	GameObject* m_pParticleEmitter{};

	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
};


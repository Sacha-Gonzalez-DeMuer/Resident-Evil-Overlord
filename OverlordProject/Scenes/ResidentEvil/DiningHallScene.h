#pragma once
#include "Prefabs/Character.h"
class RePlayerController;
class CameraSwitch;
class ReDoor;
class ReClock;
class ReGun;
class ReZombie;
class ParticleEmitterComponent;
class ReButton;
class ReMenu;
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

	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};

	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	ReZombie* m_pZombie{};
	ReClock* m_pClock{};
	ReGun* m_pGun{};
	GameObject* m_pParticleEmitter{};

	std::vector<ReButton*> m_pButtons{};
	std::vector<ReMenu*> m_pMenus{};

	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
	void AddMenus();
	void AddPostProcessing();
};


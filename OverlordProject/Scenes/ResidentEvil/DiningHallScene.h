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
class ReClassicDoor;
class DiningHallScene final : public GameScene
{
	public:
	DiningHallScene(void);
	virtual ~DiningHallScene(void);

	void Initialize() override;
	void Update() override;
	void OnGUI() override;

	void Reset();


private:
	// Player
	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};
	XMFLOAT3 m_BottomSpawnPos{ 0.f, 15.f, -90.f };
	XMFLOAT3 m_TopSpawnPos{};

	// ImGui ref
	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	std::vector<ReButton*> m_pButtons{};
	std::vector<ReMenu*> m_pMenus{};

	// Misc
	ReClassicDoor* m_pClassicDoor{};
	ReZombie* m_pZombie{};
	ReClock* m_pClock{};
	ReGun* m_pGun{};
	GameObject* m_pParticleEmitter{};

	// Audio
	FMOD::Channel* m_pAmbientChannel{};
	FMOD::Sound* m_pAmbientSound{};


	void LoadWorld();
	void AddPlayer(PxMaterial* material);
	void AddZombie(PxMaterial* material);
	void AddInput();
	void AddNavCollider(const PxMaterial& material);
	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
	void AddMenus();
	void AddPostProcessing();
	void AddGun();
	void AddClock();
	void AddSubtitles();
	void AddSound();
};


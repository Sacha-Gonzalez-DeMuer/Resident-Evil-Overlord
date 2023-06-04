#pragma once
#include "Prefabs/Character.h"
#include "Scenes/ResidentEvil/ReScene.h"

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
class ThunderController;
class DiningHallScene final : public ReScene
{
	public:
	DiningHallScene(void);
	virtual ~DiningHallScene(void);

	void Initialize() override;
	void Update() override;
	void OnGUI() override;

	void Start() override;
	void Reset() override;


private:
	// Player
	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};
	XMFLOAT3 m_SpawnPos{ };

	// ImGui ref
	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	std::vector<ReButton*> m_pButtons{};
	std::vector<ReMenu*> m_pMenus{};

	// Misc
	ThunderController* m_pThunderController{};
	ReClassicDoor* m_pClassicDoor{};
	ReZombie* m_pZombie{};
	ReClock* m_pClock{};
	ReGun* m_pGun{};
	GameObject* m_pParticleEmitter{};
	SpriteComponent* m_pStandBy{};

	// Audio
	FMOD::System* m_pFMODSys{};
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


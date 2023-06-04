#pragma once
#include "Scenes/ResidentEvil/ReScene.h"

class RePlayerController;
class CameraSwitch;
class ReDoor;
class CubePrefab;
class ReClassicDoor;
class ReGun;
class ReMenuManager;
class ThunderController;
class MainHallScene final : public ReScene
{
public:
	MainHallScene();
	~MainHallScene() override = default;
	MainHallScene(const MainHallScene& other) = delete;
	MainHallScene(MainHallScene&& other) noexcept = delete;
	MainHallScene& operator=(const MainHallScene& other) = delete;
	MainHallScene& operator=(MainHallScene&& other) noexcept = delete;

	void Initialize() override;
	void Update() override;
	void OnGUI() override;
	void PostDraw() override;

	void Start() override;
	void Reset() override;

private:
	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};
	XMFLOAT3 m_PlayerStartPos{ 0,0,0 };

	CubePrefab* m_pDebugCube{};
	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};

	// Env
	GameObject* m_pMainHall{};
	GameObject* m_pNavCollider{};

	// Misc
	ReClassicDoor* m_pClassicDoor{};
	Light m_TestLight;
	ReGun* m_pGun{};
	ThunderController* m_pThunderController{};

	//Audio
	FMOD::System* m_pFMODSys{};
	FMOD::Channel* m_pAmbientChannel{};
	FMOD::Sound* m_pAmbientSound{};

	// HUD
	ReMenuManager* m_pMenuManager{};
	SpriteComponent* m_pBackground{};

	bool m_DrawShadowMap{};
	void LoadMainMenu();

	void LoadWorld();
	void AddLights();
	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
	void AddPostProcessing();
	void AddInput();
	void AddNavCollider(const PxMaterial& material);
	void AddPlayer(PxMaterial* material);
	void AddSound();
	void AddMenu();
	void AddGun();

};

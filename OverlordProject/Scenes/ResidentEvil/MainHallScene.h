#pragma once
class RePlayerController;
class CameraSwitch;
class ReDoor;
class CubePrefab;
class ReClassicDoor;
class MainHallScene final : public GameScene
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

	//Audio
	FMOD::Channel* m_pAmbientChannel{};
	FMOD::Sound* m_pAmbientSound{};

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

	void Reset();
};

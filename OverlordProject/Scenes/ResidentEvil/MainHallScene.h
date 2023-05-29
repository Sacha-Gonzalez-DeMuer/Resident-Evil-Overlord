#pragma once
class RePlayerController;
class CameraSwitch;
class ReDoor;
class CubePrefab;
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
	GameObject* m_pMainHall{};
	GameObject* m_pNavCollider{};

	CubePrefab* m_pDebugCube{};
	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	Light m_TestLight;


	bool m_DrawShadowMap{};
	void LoadWorld();
	void AddLights();
	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
	void AddPostProcessing();
	void AddInput();
	void AddNavCollider(const PxMaterial& material);
	void AddPlayer(PxMaterial* material);
};

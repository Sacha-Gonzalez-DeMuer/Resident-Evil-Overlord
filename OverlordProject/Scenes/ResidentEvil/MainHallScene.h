#pragma once
class RePlayerController;
class CameraSwitch;
class ReDoor;
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

private:
	enum InputIds
	{
		CharacterMoveLeft,
		CharacterMoveRight,
		CharacterMoveForward,
		CharacterMoveBackward,
		CharacterJump,
		Interact
	};

	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};

	std::vector<CameraSwitch*> m_pSwitches{};
	std::vector<ReDoor*> m_pDoors{};
	std::vector<Light*> m_pLights{};

	void AddLights();
	void AddCameras();
	void AddCameraSwitches();
	void AddDoors();
};

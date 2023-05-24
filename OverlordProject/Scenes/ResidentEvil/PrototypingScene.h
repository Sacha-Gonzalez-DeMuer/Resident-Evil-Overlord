#pragma once
#include "Prefabs/Character.h"
class RePlayerController;
class CameraSwitch;
class PrototypingScene final : public GameScene
{
	public:
	PrototypingScene(void);
	virtual ~PrototypingScene(void);

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
		SnapCamTransform
	};

	RePlayerController* m_pCharacter{};
	GameObject* pPlayerObject{};

	std::vector<CameraSwitch*> m_pSwitches{};

	void AddCameras();
	void AddCameraSwitches();
};


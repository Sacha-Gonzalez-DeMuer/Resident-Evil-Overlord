#pragma once
#include <string>
#include "Misc/SpriteFont.h"

struct Subtitle
{
	SpriteFont* font{};
	std::string text{};

};

struct ReCharacterDesc
{
	PxCapsuleControllerDesc controller{};

	XMFLOAT3 spawnPosition{ 0.f, controller.height * .5f, .0f };

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };
	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };
	float rotationSpeed{ 60.f };
	float attackDistance{ 5.f };
	float maxHealth{ 100.f };
	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Interact{ -1 };
	int actionId_Sprint{ -1 };
	int actionId_Aim{ -1 };
	int actionId_Attack{ -1 };

	ReCharacterDesc(
		PxMaterial* pMaterial,
		float radius = .5f,
		float height = 2.f)
	{
		controller.setToDefault();
		controller.radius = radius;
		controller.height = height;
		controller.material = pMaterial;
	}
};

enum class ReItem
{
	GUN,
	KNIFE,
	HEALTH,
	KEY,
	EMPTY,
	COUNT
};

enum class ZState
{
	IDLE,
	WALKING,
	ATTACKING,
	DEAD
};

enum class ReMenuType
{
	MAIN,
	INGAME,
	CONTROLS,
	DEATH,
	EMPTY
};

enum InputIds
{
	CharacterMoveLeft,
	CharacterMoveRight,
	CharacterMoveForward,
	CharacterMoveBackward,
	Interact,
	CharacterSprint,
	CharacterAim,
	CharacterAttack,
	ResetScene,
	MenuUp
};

enum class ReMainHallCamera
{
	MAIN,
	DINING,
	CT1,
	STAIRS,
	ENTRANCE,
	CT2,
	STAIRS_MID,
	UPPER_MAIN,
	UPPER_DINING,
	STAIRS_BRIDGE,
	UPPER_RCORNER,
	WINDOW,

	DOORS,
};

/*
enum class ReCameraID
{
	MAIN_MAIN,
	MAIN_DINING,
	MAIN_CT1,
	MAIN_STAIRS,
	MAIN_ENTRANCE,
	MAIN_CT2,
	MAIN_STAIRS_MID,
	MAIN_UPPER_MAIN,
	MAIN_UPPER_DINING,
	MAIN_STAIRS_BRIDGE,
	MAIN_UPPER_RCORNER,
	MAIN_WINDOW,

	DINING_MAIN,
	DINING_CLOCK,
	DINING_FIREPLACE,

	DOORS,
};
*/

enum class ReScenes
{
	MAIN,
	MAIN_UPPER,
	DINING,
	DINING_UPPER,
	MENU,
	DEATH,
	ZERO
};

namespace SpawnPositions
{
	const XMFLOAT3 MainHall_Bottom{ 0,0,0 };
	const XMFLOAT3 Dining_Bottom{ 0.f, 15.f, -90.f };
}

class ReScene;
struct ReSceneData
{
	std::wstring sceneName;
	ReScene* pScene;
};
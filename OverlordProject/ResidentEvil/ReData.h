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

	XMFLOAT3 cameraOffset{ 0.f, controller.height * .5f, .0f };

	float maxMoveSpeed{ 15.f };
	float maxFallSpeed{ 15.f };
	float JumpSpeed{ 15.f };
	float moveAccelerationTime{ .3f };
	float fallAccelerationTime{ .3f };
	float rotationSpeed{ 60.f };
	float attackDistance{ 5.f };
	int actionId_MoveLeft{ -1 };
	int actionId_MoveRight{ -1 };
	int actionId_MoveForward{ -1 };
	int actionId_MoveBackward{ -1 };
	int actionId_Jump{ -1 };
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
	EMPTY
};

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

enum class ReScenes
{
	MAIN,
	DINING,
	MENU
};
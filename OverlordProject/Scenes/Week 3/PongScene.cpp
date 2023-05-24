#include "stdafx.h"
#include "PongScene.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"
#include "Prefabs/Player.h"
#include "Prefabs/PongGameSettings.h"

PongScene::PongScene()
	: GameScene{ L"PongScene" }
	, m_pGameSettings{ std::make_unique<PongGameSettings>() } 
	, m_pPlayers{}
	, m_pPongBall{nullptr}
{
	m_SceneContext.settings.enableOnGUI = true;
}

// PxVec3 constructor for XMFLOAT3
PxVec3 ToPxVec3(const XMFLOAT3& vec)
{
	return PxVec3(vec.x, vec.y, vec.z);
}

// xmfloat 3 /= operator
XMFLOAT3 operator/=(XMFLOAT3& vec, float scalar)
{
	vec.x /= scalar;
	vec.y /= scalar;
	vec.z /= scalar;
	return vec;
}


void PongScene::Initialize()
{
	const float off_side_width{ .5f };
	const float field_depth{ 5 };
	const float field_width{ 20 };
	const float field_height{ 16 };

	m_pGameSettings->Initialize(m_pPlayers, m_SceneContext, field_width/2);

	XMFLOAT3 floor_dimensions = XMFLOAT3(field_width, off_side_width, field_height);
	XMFLOAT3 right_wall_dimensions = XMFLOAT3(off_side_width, field_depth, field_height);
	XMFLOAT3 left_wall_dimensions = XMFLOAT3(off_side_width, field_depth, field_height);
	XMFLOAT3 bottom_wall_dimensions = XMFLOAT3(field_width, field_depth, off_side_width);
	XMFLOAT3 top_wall_dimensions = XMFLOAT3(field_width, field_depth, off_side_width);

	CubePrefab* floor = new CubePrefab(floor_dimensions, XMFLOAT4{ Colors::DarkCyan });
	CubePrefab* right_wall = new CubePrefab(right_wall_dimensions, XMFLOAT4{ Colors::Red });
	CubePrefab* left_wall = new CubePrefab(left_wall_dimensions, XMFLOAT4{ Colors::Red });
	CubePrefab* bottom_wall = new CubePrefab(bottom_wall_dimensions, XMFLOAT4{ Colors::Black });
	CubePrefab* top_wall = new CubePrefab(top_wall_dimensions, XMFLOAT4{ Colors::Black });


	auto pBouncyMaterial = PxGetPhysics().createMaterial(0, 0, 1);

	// Add rigidbodies to the walls and the floor
	auto floor_rb = floor->AddComponent<RigidBodyComponent>(new RigidBodyComponent(true));
	auto right_rb = right_wall->AddComponent<RigidBodyComponent>(new RigidBodyComponent(true));
	auto left_rb = left_wall->AddComponent<RigidBodyComponent>(new RigidBodyComponent(true));
	auto bot_rb = bottom_wall->AddComponent<RigidBodyComponent>(new RigidBodyComponent(true));
	auto top_rb = top_wall->AddComponent<RigidBodyComponent>(new RigidBodyComponent(true));

	// transform the dimensions into half extents
	floor_dimensions /= 2;
	right_wall_dimensions /= 2;
	left_wall_dimensions /= 2;
	bottom_wall_dimensions /= 2;
	top_wall_dimensions /= 2;

	floor_rb->AddCollider(PxBoxGeometry(floor_dimensions.x, floor_dimensions.y, floor_dimensions.z), *pBouncyMaterial);
	right_rb->AddCollider(PxBoxGeometry(right_wall_dimensions.x, right_wall_dimensions.y, right_wall_dimensions.z), *pBouncyMaterial);
	left_rb->AddCollider(PxBoxGeometry(left_wall_dimensions.x, left_wall_dimensions.y, left_wall_dimensions.z), *pBouncyMaterial);
	bot_rb->AddCollider(PxBoxGeometry(bottom_wall_dimensions.x, bottom_wall_dimensions.y, bottom_wall_dimensions.z), *pBouncyMaterial);
	top_rb->AddCollider(PxBoxGeometry(top_wall_dimensions.x, top_wall_dimensions.y, top_wall_dimensions.z), *pBouncyMaterial);

	left_wall->GetTransform()->Translate(XMFLOAT3(-field_width / 2, 0, 0));
	right_wall->GetTransform()->Translate(XMFLOAT3(field_width / 2, 0, 0));
	bottom_wall->GetTransform()->Translate(XMFLOAT3(0, 0, -field_height / 2));
	top_wall->GetTransform()->Translate(XMFLOAT3(0, 0, field_height / 2));

	AddChild(floor);
	AddChild(left_wall);
	AddChild(bottom_wall);
	AddChild(right_wall);
	AddChild(top_wall);

	// Create ball
	m_pPongBall = new SpherePrefab(0.5f, 10);
	m_pPongBall->GetTransform()->Translate(XMFLOAT3(0, .5f, 0));
	RigidBodyComponent* rb = new RigidBodyComponent();
	rb->AddCollider(PxSphereGeometry{ .5f }, *pBouncyMaterial);
	rb->SetDensity(.1f);
	rb->SetConstraint(RigidBodyConstraint::TransY, false);
	m_pPongBall->AddComponent<RigidBodyComponent>(rb);
	AddChild(m_pPongBall);
	

	// add players to scene
	for(const auto& player : m_pPlayers)
	{ 
		AddChild(player);
	}

	//Position camera 
	m_SceneContext.pCamera->GetTransform()->Translate(0, 10, 0);
}

void PongScene::HandleInput()
{
	if (m_SceneContext.pInput->IsActionTriggered(PongGameSettings::InputIds::StartGame))
	{
		m_pPongBall->GetComponent<RigidBodyComponent>()->AddForce({ 1000,0,1000 }, PxForceMode::eFORCE);
	}
}

void PongScene::Update()
{
	HandleInput();
}



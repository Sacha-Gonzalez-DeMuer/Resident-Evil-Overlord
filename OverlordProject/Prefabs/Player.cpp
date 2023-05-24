#include "stdafx.h"
#include "Player.h"
#include "Prefabs/CubePrefab.h"
#include "Prefabs/PongGameSettings.h"

Player::Player(const PongGameSettings& settings, const SceneContext& sceneContext, bool isLeftPlayer)
	: m_pInput{sceneContext.pInput}
	, m_pPeddle{ new CubePrefab({ 1, 1, 3 }, XMFLOAT4{ Colors::White }) }
	, m_moveSpeed{10.0f}
{
	Initialize(settings, isLeftPlayer);
	AddChild(m_pPeddle);
}


void Player::Initialize(const PongGameSettings& settings, bool isLeftPlayer)
{
	auto pBouncyMaterial = PxGetPhysics().createMaterial(1, 1, 0.f);
	auto rb = m_pPeddle->AddComponent(new RigidBodyComponent(false));

	XMFLOAT3 peddleScale = m_pPeddle->GetTransform()->GetScale();
	const auto leftPosition = settings.GetLeftPosition();

	auto toPos = isLeftPlayer 
		? XMVectorAdd(XMLoadFloat3(&leftPosition), XMLoadFloat3(&peddleScale))
		: -XMVectorAdd(XMLoadFloat3(&leftPosition), XMLoadFloat3(&peddleScale));


	if (isLeftPlayer)
	{
		toPos = XMVectorSetX(toPos, XMVectorGetX(toPos) - peddleScale.y * 2);		
		m_pPeddle->GetTransform()->Translate(toPos);
		m_InputUp = PongGameSettings::lInput_Up;
		m_InputDown = PongGameSettings::lInput_Down;
	}
	else
	{
		m_pPeddle->GetTransform()->Translate(toPos);
		m_InputUp = PongGameSettings::rInput_Up;
		m_InputDown = PongGameSettings::rInput_Down;
	}

	rb->AddCollider(PxBoxGeometry{ .5f,.5f,1.5f }, *pBouncyMaterial);
	rb->SetConstraint(RigidBodyConstraint::TransX, false);
	rb->SetConstraint(RigidBodyConstraint::TransY, false);
	rb->SetConstraint(RigidBodyConstraint::AllRot, false);
}

void Player::Update(const SceneContext& sceneContext)
{
	const float delta_time{ sceneContext.pGameTime->GetElapsed() };
	HandleInput(delta_time);
}

void Player::HandleInput(float deltaTime)
{
	const float const_movement_speed{ m_moveSpeed * deltaTime * 1000 };

	auto toPos = GetTransform()->GetPosition();
	// down
	if (m_pInput->IsActionTriggered(m_InputDown))
	{
		toPos.z -= const_movement_speed;
		/*if(toPos.z > -max_height)
			GetTransform()->Translate(toPos);*/

		m_pPeddle->GetComponent<RigidBodyComponent>()->AddForce({0,0,-const_movement_speed }, PxForceMode::eFORCE);
	}

	// up
	else if (m_pInput->IsActionTriggered(m_InputUp))
	{
		toPos.z += const_movement_speed;
		//if (toPos.z < m_maxHeight)
			//GetTransform()->Translate(toPos);

		m_pPeddle->GetComponent<RigidBodyComponent>()->AddForce({ 0,0,const_movement_speed }, PxForceMode::eFORCE);

	}
}
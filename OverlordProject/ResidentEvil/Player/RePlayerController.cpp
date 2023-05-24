#include "stdafx.h"
#include "RePlayerController.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/ReCameraManager.h"

RePlayerController::RePlayerController(const ReCharacterDesc& characterDesc) 
	: m_CharacterDesc(characterDesc),
m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{
}

void RePlayerController::Initialize(const SceneContext&)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	SetTag(L"Player");

	ReCameraManager::Get().OnCameraSwitch.AddFunction([this]() {
		OnCamSwitch();
		});
}

void RePlayerController::Update(const SceneContext& sceneContext)
{
	if (sceneContext.pCamera->IsActive())
	{
		constexpr float epsilon{ 0.01f };
		const float deltaTime{ sceneContext.pGameTime->GetElapsed() };

		const auto pInput = sceneContext.pInput;

		XMFLOAT2 move;

		int moveForward = pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward);
		int moveBackward = pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward);
		int moveLeft = pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft);
		int moveRight = pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight);

		move.x = static_cast<float>(moveRight - moveLeft);
		move.y = static_cast<float>(moveForward - moveBackward);

		if (abs(move.y) < epsilon)
			move.y = InputManager::GetThumbstickPosition(true).y;

		if (abs(move.x) < epsilon)
			move.x = InputManager::GetThumbstickPosition(true).x;


		//************************
		//GATHER TRANSFORM INFO
		const auto pTransform = m_pControllerComponent->GetTransform();
		const auto& forward = pTransform->GetForward();
		const auto& right = pTransform->GetRight();
		// print cam forward vec

		////********
		////MOVEMENT
		const auto acceleration = m_MoveAcceleration * deltaTime;
		if (move.x != 0.f || move.y != 0.f)
		{
			XMVECTOR forwardVec = XMLoadFloat3(&forward);
			XMVECTOR rightVec = XMLoadFloat3(&right);

			// Scale the camera's forward and right vectors based on movement input
			XMVECTOR forwardScaled = XMVectorScale(m_CamForward, move.y);
			XMVECTOR rightScaled = XMVectorScale(m_CamRight, move.x);

			// Add the scaled vectors together to get the adjusted movement direction
			XMVECTOR dirVec = XMVectorAdd(forwardScaled, rightScaled);

			XMStoreFloat3(&m_CurrentDirection, dirVec);

			m_MoveSpeed += acceleration;
			m_MoveSpeed = std::min(m_MoveSpeed, m_CharacterDesc.maxMoveSpeed);
		}
		else
		{
			m_MoveSpeed -= acceleration;
			m_MoveSpeed = std::max(m_MoveSpeed, 0.f);
		}


		const auto horizontalVelocity = XMVectorScale(XMLoadFloat3(&m_CurrentDirection), m_MoveSpeed);
		m_TotalVelocity.x = XMVectorGetX(horizontalVelocity);
		m_TotalVelocity.z = XMVectorGetZ(horizontalVelocity);

		const auto collisions = m_pControllerComponent->GetCollisionFlags();
		const auto isGrounded = collisions.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN);

		if (!isGrounded)
		{
			m_TotalVelocity.y -= m_FallAcceleration * deltaTime;
			m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_CharacterDesc.maxFallSpeed);
		}
		else
			m_TotalVelocity.y = 0.f;

		//************
		//DISPLACEMENT
		auto displacement = XMVectorScale(XMLoadFloat3(&m_TotalVelocity), deltaTime);

		XMFLOAT3 displacementFloat3;
		XMStoreFloat3(&displacementFloat3, displacement);

		m_pControllerComponent->Move(displacementFloat3, 0.001f);
	}
}

void RePlayerController::OnCamSwitch()
{
	auto& camManager = ReCameraManager::Get();
	const auto& activeCam = camManager.GetActiveCamera();

	if (activeCam)
	{
		m_CamForward = activeCam->GetLookAt();
		m_CamRight = activeCam->GetRight();
	}
}

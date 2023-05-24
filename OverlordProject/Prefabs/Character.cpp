#include "stdafx.h"
#include "Character.h"
#include <algorithm>
#include "Materials/DiffuseMaterial_Skinned.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));

	//Camera
	const auto pCamera = AddChild(new FixedCamera());
	m_pCameraComponent = pCamera->GetComponent<CameraComponent>();
	m_pCameraComponent->SetActive(true); //Uncomment to make this camera the active camera

	m_pCameraComponent->GetTransform()->Translate(m_CharacterDesc.cameraOffset);
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_pCameraComponent->IsActive())
	{
		constexpr float epsilon{ 0.01f };

		//***************
		//HANDLE INPUT
		const auto pInput = sceneContext.pInput;

		//## Input Gathering (move)
		XMFLOAT2 move; //Uncomment

		int moveForward = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward);
		int moveBackward = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward);
		int moveLeft = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft);
		int moveRight = sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight);

		move.x = static_cast<float>(moveRight - moveLeft);
		move.y = static_cast<float>(moveForward - moveBackward);

		if (abs(move.y) < epsilon)
			move.y = InputManager::GetThumbstickPosition(true).y;

		if (abs(move.x) < epsilon)
			move.x = InputManager::GetThumbstickPosition(true).x;

		//## Input Gathering (look)
		XMFLOAT2 look{ 0.f, 0.f };
		if (InputManager::IsMouseButton(InputState::down, VK_LBUTTON))
		{
			auto mouseMovement = pInput->GetMouseMovement();
			look.x = static_cast<float>(mouseMovement.x);
			look.y = static_cast<float>(mouseMovement.y);

			if(abs(look.x) < epsilon && abs(look.y) < epsilon)
				look = InputManager::GetThumbstickPosition(false);
		}


		//************************
		//GATHERING TRANSFORM INFO
		const auto pTransform = m_pControllerComponent->GetTransform();
		const auto& forward = pTransform->GetForward();
		const auto& right = pTransform->GetRight();

		//***************
		//CAMERA ROTATION
		const auto deltaTime = sceneContext.pGameTime->GetElapsed();
		const auto rotationSpeed = m_CharacterDesc.rotationSpeed * deltaTime;
		m_TotalYaw += look.x * rotationSpeed;
		m_TotalPitch += look.y * rotationSpeed;

		GetTransform()->Rotate(m_TotalPitch, m_TotalYaw, 0);

		////********
		////MOVEMENT
		const auto acceleration = m_MoveAcceleration * deltaTime;
		if (move.x != 0.f || move.y != 0.f)
		{
			XMVECTOR forwardVec = XMLoadFloat3(&forward);
			XMVECTOR rightVec = XMLoadFloat3(&right);

			XMVECTOR forwardScaled = XMVectorScale(forwardVec, move.y);
			XMVECTOR rightScaled = XMVectorScale(rightVec, move.x);

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
		else if (pInput->IsActionTriggered(m_CharacterDesc.actionId_Jump))
		{
			m_TotalVelocity.y = m_CharacterDesc.JumpSpeed;
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

void Character::DrawImGui()
{
	if (ImGui::CollapsingHeader("Character"))
	{
		ImGui::Text(std::format("Move Speed: {:0.1f} m/s", m_MoveSpeed).c_str());
		ImGui::Text(std::format("Fall Speed: {:0.1f} m/s", m_TotalVelocity.y).c_str());

		ImGui::Text(std::format("Move Acceleration: {:0.1f} m/s2", m_MoveAcceleration).c_str());
		ImGui::Text(std::format("Fall Acceleration: {:0.1f} m/s2", m_FallAcceleration).c_str());

		const float jumpMaxTime = m_CharacterDesc.JumpSpeed / m_FallAcceleration;
		const float jumpMaxHeight = (m_CharacterDesc.JumpSpeed * jumpMaxTime) - (0.5f * (m_FallAcceleration * powf(jumpMaxTime, 2)));
		ImGui::Text(std::format("Jump Height: {:0.1f} m", jumpMaxHeight).c_str());

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Move Speed (m/s)", &m_CharacterDesc.maxMoveSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Move Acceleration Time (s)", &m_CharacterDesc.moveAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_MoveAcceleration = m_CharacterDesc.maxMoveSpeed / m_CharacterDesc.moveAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		if (ImGui::DragFloat("Max Fall Speed (m/s)", &m_CharacterDesc.maxFallSpeed, 0.1f, 0.f, 0.f, "%.1f") ||
			ImGui::DragFloat("Fall Acceleration Time (s)", &m_CharacterDesc.fallAccelerationTime, 0.1f, 0.f, 0.f, "%.1f"))
		{
			m_FallAcceleration = m_CharacterDesc.maxFallSpeed / m_CharacterDesc.fallAccelerationTime;
		}

		ImGui::Dummy({ 0.f,5.f });
		ImGui::DragFloat("Jump Speed", &m_CharacterDesc.JumpSpeed, 0.1f, 0.f, 0.f, "%.1f");
		ImGui::DragFloat("Rotation Speed (deg/s)", &m_CharacterDesc.rotationSpeed, 0.1f, 0.f, 0.f, "%.1f");

		bool isActive = m_pCameraComponent->IsActive();
		if(ImGui::Checkbox("Character Camera", &isActive))
		{
			m_pCameraComponent->SetActive(isActive);
		}
	}
}
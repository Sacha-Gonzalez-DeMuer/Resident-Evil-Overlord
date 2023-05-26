#include "stdafx.h"
#include "RePlayerController.h"
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/ReCameraManager.h"
#include "FilePaths.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "ResidentEvil/World/InteractableComponent.h"
#include "ResidentEvil/Player/RePlayerAnimController.h"
#include "ResidentEvil/Items/ReInventory.h"

RePlayerController::RePlayerController(const ReCharacterDesc& characterDesc) 
	: m_CharacterDesc(characterDesc),
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{ }

void RePlayerController::Initialize(const SceneContext& sceneContext)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	SetTag(L"Player");

	ReCameraManager::Get().OnCameraSwitch.AddFunction([this]() {
		OnCamSwitch();
		});

	ModelComponent* modelComponent;
	m_pModelObject = AddChild(new GameObject());
	// Player Model
	if (sceneContext.useDeferredRendering)
	{
		const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
		pSkinnedMaterial->SetDiffuseTexture(FilePath::SOLDIER_BODY_DIFFUSE);
		modelComponent = m_pModelObject->AddComponent(new ModelComponent(FilePath::SOLDIER_ANIMS_OVM));
		modelComponent->SetMaterial(pSkinnedMaterial);

	}
	else
	{
		const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>();
		pSkinnedMaterial->SetDiffuseTexture(FilePath::SOLDIER_BODY_DIFFUSE);
		modelComponent = m_pModelObject->AddComponent(new ModelComponent(FilePath::SOLDIER_ANIMS_OVM));
		modelComponent->SetMaterial(pSkinnedMaterial);
	}

	m_pModelObject->GetTransform()->Scale(0.11f, 0.11f, 0.11f);
	m_pModelObject->GetTransform()->Translate(0.f, -m_CharacterDesc.controller.height / 2, 0.f);


	// Animation
	m_pAnimController = AddComponent(new RePlayerAnimController(modelComponent->GetAnimator(), this));


	// Inventory
	m_pInventory = AddComponent(new ReInventory());
}

void RePlayerController::Update(const SceneContext& sceneContext)
{
	if (sceneContext.pCamera->IsActive())
	{
		constexpr float epsilon{ 0.01f };
		const float deltaTime{ sceneContext.pGameTime->GetElapsed() };

		const auto pInput = sceneContext.pInput;
		HandleInputActions(pInput);

		XMFLOAT2 move{};

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
		//const auto& right = pTransform->GetRight();

		////********
		//// ROTATION
		//// Resident Evil made use of tank controls
		if (move.x > 0.f)
			m_TotalYaw += m_CharacterDesc.rotationSpeed * deltaTime;
		else if (move.x < 0.f)
			m_TotalYaw -= m_CharacterDesc.rotationSpeed * deltaTime;
		GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);

		////********
		////MOVEMENT
		const auto acceleration = m_MoveAcceleration * deltaTime;
		m_IsMoving = abs(move.y) > epsilon;
		if (m_IsMoving)
		{
			XMVECTOR forwardScaled = XMVectorScale(XMLoadFloat3(&forward), -move.y);
			XMStoreFloat3(&m_CurrentDirection, forwardScaled);

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
		auto isGrounded = collisions.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN);

		if (!isGrounded)
		{
			m_TotalVelocity.y -= m_FallAcceleration * deltaTime;
			m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_CharacterDesc.maxFallSpeed);
		}
		else
			m_TotalVelocity.y = 0.f;

		//************
		//DISPLACEMENT
		float scale = deltaTime;
		if (pInput->IsActionTriggered(m_CharacterDesc.actionId_Sprint))
			scale *= 2;

		auto displacement = XMVectorScale(XMLoadFloat3(&m_TotalVelocity), scale);

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

void RePlayerController::HandleInputActions(InputManager* input)
{
	if(input->IsActionTriggered(m_CharacterDesc.actionId_Interact))
		Interact();

	if (m_pInventory->HasGun() && input->IsActionTriggered(m_CharacterDesc.actionId_Aim))
	{
		m_pInventory->EquipItem(ReItem::GUN);
		m_IsAiming = true;
	}
	else m_IsAiming = false;
}

void RePlayerController::Interact()
{
	const auto& origin = m_pControllerComponent->GetPosition();
	PxVec3 pxOrigin = { origin.x, origin.y, origin.z };

	const auto& forward = m_pControllerComponent->GetTransform()->GetForward();
	PxVec3 pxForward = { forward.x, forward.y, forward.z };

	PxRaycastBuffer hit;
	PxQueryFilterData filterData{};

	if (GetScene()->GetPhysxProxy()
		->Raycast(pxOrigin, pxForward, 10.f, hit, PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.hasBlock)
		{
			auto go = static_cast<BaseComponent*>(hit.block.actor->userData)->GetGameObject();
			auto pInteractable = go->GetComponent<InteractableComponent>();
			if (pInteractable)
				pInteractable->Interact();
		}
	}

}
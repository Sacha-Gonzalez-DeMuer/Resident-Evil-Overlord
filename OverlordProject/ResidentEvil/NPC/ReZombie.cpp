#include "stdafx.h"
#include "ReZombie.h"
#include "FilePaths.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "ResidentEvil/NPC/ReZombieAnimController.h"
#include "ResidentEvil/ReData.h"

ReZombie::ReZombie(const ReCharacterDesc& characterDesc)
	: m_CharacterDesc(characterDesc)
{
}

void ReZombie::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
	SetTag(L"Zombie");


	// Zombie Model
	const auto pSkinnedMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Skinned>();
	pSkinnedMaterial->SetDiffuseTexture(FilePath::SOLDIER_BODY_DIFFUSE);

	m_pModelObject = AddChild(new GameObject());
	auto modelComponent = m_pModelObject->AddComponent(new ModelComponent(FilePath::SOLDIER_ANIMS_OVM));
	modelComponent->SetMaterial(pSkinnedMaterial);
	m_pModelObject->GetTransform()->Scale(0.11f, 0.11f, 0.11f);
	m_pModelObject->GetTransform()->Translate(0.f, -m_CharacterDesc.controller.height / 2, 0.f);

	m_pAnimController = AddComponent(new ReZombieAnimController(modelComponent->GetAnimator(), this));
}

void ReZombie::Update(const SceneContext& sceneContext)
{

	if (sceneContext.pCamera->IsActive())
	{
		//constexpr float epsilon{ 0.01f };
		const float deltaTime{ sceneContext.pGameTime->GetElapsed() };

		// scan for player
		const auto& targetPos = m_pTarget->GetTransform()->GetPosition();
		const auto& myPos = GetTransform()->GetPosition();
		const auto& difference = XMVectorSubtract(XMLoadFloat3(&targetPos), XMLoadFloat3(&myPos));
		float distance = XMVectorGetX(XMVector3Length(difference));
		if (distance > 30.f) return;

		const auto& dispacement = XMVectorScale(XMVector3Normalize(difference), m_CharacterDesc.maxMoveSpeed);
		XMFLOAT3 fdisplacement;
		XMStoreFloat3(&fdisplacement, dispacement);


		// if player is in range, move towards player
		if (distance < 20.f && distance > m_CharacterDesc.attackDistance)
		{
			m_State = ZState::WALKING;
			m_pControllerComponent->Move(fdisplacement, deltaTime);
		}
		else if (distance < m_CharacterDesc.attackDistance)
		{
			m_State = ZState::ATTACKING;
			// trigger attack anim on player and zombie, lock player and zombie into fixed relative pos
		}
		else
		{
			m_State = ZState::IDLE;
		}


	}
}

void ReZombie::Attack()
{
	m_pAnimController->TriggerState(ZState::ATTACKING);

	// lock target into position relative to zombie
	const auto& zPos = GetTransform()->GetPosition();
	const auto& tPos = m_pTarget->GetTransform()->GetPosition();
	const auto& difference = XMVectorSubtract(XMLoadFloat3(&tPos), XMLoadFloat3(&zPos));
	const auto& dispacement = XMVectorScale(XMVector3Normalize(difference), m_CharacterDesc.attackDistance);
	XMFLOAT3 fdisplacement;
	XMStoreFloat3(&fdisplacement, dispacement);
	m_pTarget->GetTransform()->Translate(fdisplacement.x, fdisplacement.y, fdisplacement.z);


}

void ReZombie::UpdateBehavior(const SceneContext& /*sceneContext*/)
{

}


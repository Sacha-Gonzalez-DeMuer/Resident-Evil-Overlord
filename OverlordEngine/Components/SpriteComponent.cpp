#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color) :
	m_SpriteAsset(spriteAsset),
	m_Pivot(pivot),
	m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& /*sceneContext*/)
{
	if (!m_pTexture)
		return;

	// Get the sprite renderer instance
	auto pSpriteRenderer = SpriteRenderer::Get();

	// Get the owning GameObject's transform
	const auto& transform = m_pGameObject->GetTransform();

	// Get the position of the sprite
	const auto& position = transform->GetPosition();
	const auto& worldPosition = transform->GetWorldPosition();

	// Get the scale of the sprite
	const XMFLOAT2 scale = { transform->GetScale().x, transform->GetScale().y };

	// Get the rotation of the sprite
	const XMFLOAT3 rotation{ MathHelper::QuaternionToEuler(transform->GetRotation()) };

	// Draw the sprite with the sprite renderer
	pSpriteRenderer->AppendSprite(
		m_pTexture,
		{ worldPosition.x, worldPosition.y },
		m_Color, m_Pivot,
		scale,
		rotation.z, position.z);
}
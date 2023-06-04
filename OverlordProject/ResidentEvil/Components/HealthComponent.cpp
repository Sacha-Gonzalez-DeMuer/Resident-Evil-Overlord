#include "stdafx.h"

#include "HealthComponent.h"

HealthComponent::HealthComponent(float health)
	: m_Health(health), m_MaxHealth(health)
{
}

void HealthComponent::TakeDamage(float damage)
{
	m_Health -= damage;
	if (m_Health <= 0)
	{
		m_Health = 0;
		OnDeath.Invoke();
	}
	else
		OnTakeDamage.Invoke();
}


void HealthComponent::Reset()
{
	m_Health = m_MaxHealth;
}

void HealthComponent::Initialize(const SceneContext& )
{
}

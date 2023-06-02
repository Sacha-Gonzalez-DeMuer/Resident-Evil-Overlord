#include "stdafx.h"
#include "Delegate.h"

class HealthComponent : public BaseComponent
{
public:
	HealthComponent(float health = 100.f);
	~HealthComponent() override = default;
	HealthComponent(const HealthComponent& other) = delete;
	HealthComponent(HealthComponent&& other) noexcept = delete;
	HealthComponent& operator=(const HealthComponent& other) = delete;
	HealthComponent& operator=(HealthComponent&& other) noexcept = delete;
	void SetHealth(float health) { m_Health = health; }
	float GetHealth() const { return m_Health; }
	void TakeDamage(float damage);
	void Heal(float heal);
	bool IsDead() const { return m_Health <= 0.f; }

	Delegate<> OnDeath;
	Delegate<> OnTakeDamage;

protected:
	void Initialize(const SceneContext& sceneContext) override;
private:
	float m_Health;
	float m_MaxHealth;
};
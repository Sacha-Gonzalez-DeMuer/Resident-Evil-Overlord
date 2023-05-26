#include "stdafx.h"
#include "ReInventory.h"

ReInventory::ReInventory()
	: m_EquippedItem(ReItem::GUN)
	, m_Items{static_cast<UINT>(ReItem::COUNT)}
{
}

void ReInventory::EquipItem(const ReItem& item)
{
	if (m_EquippedItem == item) return;

	if (m_Items[static_cast<UINT>(item)] > 0)
		m_EquippedItem = item;
}

bool ReInventory::HasGun() const
{
	return m_Items[static_cast<UINT>(ReItem::GUN)] > 0;
}

void ReInventory::AddItem(const ReItem& item)
{
	++m_Items[static_cast<UINT>(item)];
}

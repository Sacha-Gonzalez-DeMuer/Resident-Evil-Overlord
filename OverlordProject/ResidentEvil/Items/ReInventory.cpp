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
	if(m_Items.empty()) return false;
	return m_Items[static_cast<UINT>(ReItem::GUN)] > 0;
}

void ReInventory::AddItem(const ReItem& item)
{
	Logger::LogInfo(L"ReInventory::AddItem()" + static_cast<UINT>(item));
	++m_Items[static_cast<UINT>(item)];
}

int ReInventory::GetItemAmount(const ReItem& item) const
{
	return m_Items[static_cast<UINT>(item)];
}

void ReInventory::Reset()
{
	m_Items.clear();
	m_EquippedItem = ReItem::EMPTY;
}

#include "stdafx.h"
#include "ReGun.h"
#include "ResidentEvil/Items/ReInventory.h"

ReGun::ReGun(const XMFLOAT3& pos, const XMFLOAT3& size)
	: ReInteractableObject(pos, size)
{}

void ReGun::Interact()
{
	m_pDestInventory->AddItem(ReItem::GUN);
}

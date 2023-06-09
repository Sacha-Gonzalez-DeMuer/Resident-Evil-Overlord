#include "stdafx.h"
#include "ReDoor.h"
#include <iostream>
#include "Prefabs/CubePrefab.h"
#include "ResidentEvil/World/InteractableComponent.h"

ReDoor::ReDoor(const XMFLOAT3& pos, const XMFLOAT3& size)
	: ReInteractableObject(pos, size)
{}


void ReDoor::Interact()
{
	OnInteract.Invoke();
}
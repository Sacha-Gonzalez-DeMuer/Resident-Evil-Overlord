#include "stdafx.h"
#include "BoneObject.h"
#include <DirectXMath.h>

BoneObject::BoneObject(BaseMaterial* pMaterial, float length) :
	m_Length(length),
	m_pMaterial(pMaterial)
{
}

void BoneObject::AddBone(BoneObject* pBone)
{
	// translate pbone along x-axis so it is located next to the parent
	pBone->GetTransform()->Translate(m_Length, 0, 0);

	// add pbone as child to the boneobject
	AddChild(pBone);
}

void BoneObject::CalculateBindPose()
{ 
	DirectX::XMMATRIX worldMatrix{ DirectX::XMLoadFloat4x4(&GetTransform()->GetWorld()) };
	DirectX::XMMATRIX bindPose{ DirectX::XMMatrixInverse(nullptr, worldMatrix) };

	XMStoreFloat4x4(&m_BindPose, bindPose);
}

void BoneObject::Initialize(const SceneContext&)
{
	// create an empty gameobject
	auto pEmpty = new GameObject();

	// add pEmpty as child to the boneobject
	AddChild(pEmpty);

	// add a modelcomponent to pempty (pModel), use bone.ovm
	auto pModel = new ModelComponent(L"Meshes/Bone.ovm");
	pEmpty->AddComponent(pModel);

	// set the material of the modelcomponent to m_pMaterial
	pModel->SetMaterial(m_pMaterial);

	// rotate pempty -90 degrees around the y axis
	pEmpty->GetTransform()->Rotate(0, -90, 0);

	// scale pempty to m_Length
	pEmpty->GetTransform()->Scale(m_Length, m_Length, m_Length);
}

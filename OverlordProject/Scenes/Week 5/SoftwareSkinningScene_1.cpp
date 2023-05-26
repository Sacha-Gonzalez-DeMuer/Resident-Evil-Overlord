#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"

SoftwareSkinningScene_1::SoftwareSkinningScene_1() :
	GameScene(L"SoftwareSkinningScene_1"), m_pBone0(nullptr), m_pBone1(nullptr)
{
}

void SoftwareSkinningScene_1::Initialize()
{
	// create a colormaterial to use for both boneobjects
	auto color_material{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };

	// create a new gameobject as root of the skeleton
	auto pRoot = new GameObject();

	// create a boneobject as child of pRoot
	m_pBone0 = new BoneObject(color_material, 15.f);
	pRoot->AddChild(m_pBone0);

	// create a boneobject as child of m_pBone0
	m_pBone1 = new BoneObject(color_material, 15.f);
	m_pBone0->AddBone(m_pBone1);

	// add pRoot to the scene
	AddChild(pRoot);


	// enable gui
	m_SceneContext.settings.enableOnGUI = true;
}

void SoftwareSkinningScene_1::Update()
{
	// rotate m_pBone0 around the y-axis
	m_pBone0->GetTransform()->Rotate(0, 0, m_Bone0Rotation);
	// rotate m_pBone1 around the y-axis
	m_pBone1->GetTransform()->Rotate(0, 0, m_Bone1Rotation);


	// update the rotation

	if (m_AutoRotate)
	{
		m_Bone0Rotation += m_RotationSign * m_SceneContext.pGameTime->GetElapsed();
		m_Bone1Rotation = m_Bone0Rotation;

		// check if the rotation is too high or too low
		if (m_Bone0Rotation > 45.f || m_Bone0Rotation < -45.f)
		{
			// change the sign of the rotation
			m_RotationSign *= -1;
		}
	}
}


void SoftwareSkinningScene_1::OnGUI()
{

	if (ImGui::Begin("Software Skinning Scene 1"))
	{
		// title
		ImGui::Text("Software Skinning Scene 1");

		// imgui widget to manually adjust rotations of both bones
		ImGui::SliderFloat("Bone 0 Rotation", &m_Bone0Rotation, -45.f, 45.f);
		ImGui::SliderFloat("Bone 1 Rotation", &m_Bone1Rotation, -45.f, 45.f);

		/// checkbox to toggle between manual and automatic mode
		ImGui::Checkbox("Automatic Rotation", &m_AutoRotate);


		ImGui::End();
	}
}



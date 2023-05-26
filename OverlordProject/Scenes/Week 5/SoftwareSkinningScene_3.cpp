#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Prefabs/BoneObject.h"
#include "Materials/ColorMaterial.h"
#include "Prefabs/BoneObject.h"


SoftwareSkinningScene_3::SoftwareSkinningScene_3()
	: GameScene{ L"SoftwareSkinningScene_3" }
	, m_pBone0{ nullptr }
	, m_pBone1{ nullptr }
	, m_pMeshDrawComponent{ nullptr }
	, m_SkinnedVertices{}
	, m_BoneRotation{}
	, m_RotationSign{ 1 }
	, m_Bone0Rotation{}
	, m_Bone1Rotation{}
{
}

// initialize
void SoftwareSkinningScene_3::Initialize()
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


	m_pMeshDrawComponent = new MeshDrawComponent(24, true);
	m_pBone0->CalculateBindPose();
	m_pBone1->CalculateBindPose();
	auto pBoxDrawer = new GameObject();
	pBoxDrawer->AddComponent(m_pMeshDrawComponent);

	AddChild(pBoxDrawer);

	InitializeVertices(15.f);

	// enable gui
	m_SceneContext.settings.enableOnGUI = true;
}

void SoftwareSkinningScene_3::Update()
{
	m_pBone0->GetTransform()->Rotate(m_Bone0Rotation, true);
	m_pBone1->GetTransform()->Rotate(m_Bone1Rotation, true);

	XMFLOAT4X4 boneTransforms[2];
	// get the inverse of the bone bindposes
	boneTransforms[0] = m_pBone0->GetBindPose();
	boneTransforms[1] = m_pBone1->GetBindPose();

	// multiply bonetransforms with world matrices of corresponding bones
	XMStoreFloat4x4(&boneTransforms[0], XMLoadFloat4x4(&boneTransforms[0]) * XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld()));
	XMStoreFloat4x4(&boneTransforms[1], XMLoadFloat4x4(&boneTransforms[1]) * XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld()));

	DirectX::XMMATRIX boneTransform0{ XMLoadFloat4x4(&boneTransforms[0]) };
	DirectX::XMMATRIX boneTransform1{ XMLoadFloat4x4(&boneTransforms[1]) };

	// MeshDrawer Update
	m_pMeshDrawComponent->RemoveTriangles();
	for (int i = 0; i < m_SkinnedVertices.size(); i += 4)
	{
		for (int j = 0; j < 4; ++j)
		{
			// check if blendweights are 0
			if(m_SkinnedVertices[i+j].blendWeight0 < 1 && m_SkinnedVertices[i + j].blendWeight1 == 0)
				continue;

			// Transform the vertex position with the bone transform
			XMVECTOR position = XMLoadFloat3(&m_SkinnedVertices[i + j].originalVertex.Position);
			XMVECTOR transformedPosition0 = XMVector3Transform(position, boneTransform0);
			XMVECTOR transformedPosition1 = XMVector3Transform(position, boneTransform1);

			XMVECTOR interpolatedPosition = transformedPosition0 * m_SkinnedVertices[i + j].blendWeight0 + transformedPosition1 * m_SkinnedVertices[i + j].blendWeight1;

			// assign position to corresponding vertex
			XMStoreFloat3(&m_SkinnedVertices[i + j].transformedVertex.Position, interpolatedPosition);
		}

		// make QuadPosNormCol from 4 SkinnedVertex
		QuadPosNormCol quad;
		quad.Vertex1 = m_SkinnedVertices[i].transformedVertex;
		quad.Vertex2 = m_SkinnedVertices[i + 1].transformedVertex;
		quad.Vertex3 = m_SkinnedVertices[i + 2].transformedVertex;
		quad.Vertex4 = m_SkinnedVertices[i + 3].transformedVertex;

		// update the buffer of the meshdrawer
		m_pMeshDrawComponent->AddQuad(quad);
	}

	m_pMeshDrawComponent->UpdateBuffer();

	if (m_AutoRotate)
	{
		m_Bone0Rotation.z += m_RotationSign * m_SceneContext.pGameTime->GetElapsed();
		m_Bone1Rotation = m_Bone0Rotation;

		// check if the rotation is too high or too low
		if (m_Bone0Rotation.z > 45.f || m_Bone0Rotation.z < -45.f)
		{
			// change the sign of the rotation
			m_RotationSign *= -1;
		}
	}
}

void SoftwareSkinningScene_3::OnGUI()
{
	if (ImGui::Begin("Software Skinning Scene 1"))
	{
		// title
		ImGui::Text("Software Skinning Scene 1");

		// imgui widget to manually adjust rotations of both bones
		ImGui::SliderFloat("Bone 0 Rotation", &m_Bone0Rotation.z, -45.f, 45.f);
		ImGui::SliderFloat("Bone 1 Rotation", &m_Bone1Rotation.z, -45.f, 45.f);

		/// checkbox to toggle between manual and automatic mode
		ImGui::Checkbox("Automatic Rotation", &m_AutoRotate);

		ImGui::End();
	}
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);
#pragma region BOX 1
	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f, 0.f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.0f, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.0f, 0.f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };
#pragma region BOX 2
	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, .5f, .5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, .5f, .5f);
	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f, 1.f);
#pragma endregion
}


#include "stdafx.h"
#include "DiffuseMaterial_Shadow_Skinned.h"

DiffuseMaterial_Shadow_Skinned::DiffuseMaterial_Shadow_Skinned():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow_Skinned.fx")
{}

void DiffuseMaterial_Shadow_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	/*
	 * TODO_W8
	 * Update The Shader Variables
	 * 1. Update the LightWVP > Used to Transform a vertex into Light clipping space
	 * 	LightWVP = model_world * light_viewprojection
	 * 	(light_viewprojection [LightVP] can be acquired from the ShadowMapRenderer)
	 * 
	 * 2. Update the ShadowMap texture
	 * 
	 * 3. Update the Light Direction (retrieve the direction from the LightManager > sceneContext)
	 * 
	 * 4. Update Bones
	*/

	const auto pShadowMapRenderer = ShadowMapRenderer::Get();
	if (!pShadowMapRenderer)
	{
		Logger::LogError(L"DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables > ShadowMapRenderer is null!");
		return;
	}

	// 1.
	const auto pLightVP = pShadowMapRenderer->GetLightVP();
	const auto modelWorld = pModel->GetTransform()->GetWorld();
	XMFLOAT4X4 lightWVP;
	XMStoreFloat4x4(&lightWVP, XMMatrixMultiply(XMLoadFloat4x4(&pLightVP), XMLoadFloat4x4(&modelWorld)));
	SetVariable_Matrix(L"gWorldViewProj_Light", lightWVP);

	// 2.
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	// 3.
	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);


	// 4.
	const auto pAnimator = pModel->GetAnimator();
	if (!pAnimator)
	{
		Logger::LogError(L"DiffuseMaterial_Shadow_Skinned::OnUpdateModelVariables > ModelAnimator is null!");
		return;
	}

	std::vector<XMFLOAT4X4> boneTransforms = pAnimator->GetBoneTransforms();
	SetVariable_MatrixArray(L"gBones", reinterpret_cast<float*>(boneTransforms.data()), static_cast<UINT>(boneTransforms.size()));


}

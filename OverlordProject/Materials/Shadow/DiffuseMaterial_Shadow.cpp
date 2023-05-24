#include "stdafx.h"
#include "DiffuseMaterial_Shadow.h"

DiffuseMaterial_Shadow::DiffuseMaterial_Shadow():
	Material(L"Effects/Shadow/PosNormTex3D_Shadow.fx")
{}

void DiffuseMaterial_Shadow::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Shadow::InitializeEffectVariables()
{
	SetVariable_Scalar(L"gShadowMapBias", .01f);
}

void DiffuseMaterial_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
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
	*/

	const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	//Update LightWVP Matrix
	const XMFLOAT4X4 lightVP = pShadowMapRenderer->GetLightVP();
	const XMFLOAT4X4 modelWorld = pModel->GetTransform()->GetWorld();

	XMFLOAT4X4 lightWorldViewProj;
	XMStoreFloat4x4(&lightWorldViewProj, XMMatrixMultiply(XMLoadFloat4x4(&modelWorld), XMLoadFloat4x4(&lightVP)));
	SetVariable_Matrix(L"gWorldViewProj_Light", lightWorldViewProj);

	//Update the shadowmap texture
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());

	// Update the light direction
	const auto pLightManager{ sceneContext.pLights };
	const auto lightDirection{ pLightManager->GetDirectionalLight().direction };
	SetVariable_Vector(L"gLightDirection", lightDirection);
	
	SetVariable_Matrix(L"gWorld", pModel->GetTransform()->GetWorld());

	// get world view projection matrix
	const XMFLOAT4X4 viewProjection{ sceneContext.pCamera->GetViewProjection() };
	XMFLOAT4X4 worldViewProjection;
	XMStoreFloat4x4(&worldViewProjection, XMMatrixMultiply(XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()), XMLoadFloat4x4(&viewProjection)));
	SetVariable_Matrix(L"gWorldViewProj", worldViewProjection);

}
#include "stdafx.h"
#include "BasicMaterial_Deferred_Shadow.h"

BasicMaterial_Deferred_Shadow::BasicMaterial_Deferred_Shadow() :
	Material(L"Effects/Deferred/Shadow/BasicEffect_Shadow_Deferred.fx")
{
}

void BasicMaterial_Deferred_Shadow::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void BasicMaterial_Deferred_Shadow::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void BasicMaterial_Deferred_Shadow::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred_Shadow::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void BasicMaterial_Deferred_Shadow::SetShadowMap(TextureData* pTextureData)
{
	SetVariable_Texture(L"gShadowMap", pTextureData);
}

void BasicMaterial_Deferred_Shadow::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred_Shadow::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void BasicMaterial_Deferred_Shadow::SetSpecularColor(const XMFLOAT4& color)
{
	SetVariable_Vector(L"gSpecularColor", color);
}

void BasicMaterial_Deferred_Shadow::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void BasicMaterial_Deferred_Shadow::InitializeEffectVariables()
{
}

void BasicMaterial_Deferred_Shadow::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	const auto pShadowMapRenderer = ShadowMapRenderer::Get();

	const auto& lightVP = pShadowMapRenderer->GetLightVP();
	const auto& modelWorld = pModel->GetTransform()->GetWorld();

	XMFLOAT4X4 lightWorldViewProj;
	XMStoreFloat4x4(&lightWorldViewProj, XMMatrixMultiply(XMLoadFloat4x4(&modelWorld), XMLoadFloat4x4(&lightVP)));
	SetVariable_Matrix(L"gWorldViewProj_Light", lightWorldViewProj);
	SetVariable_Texture(L"gShadowMap", pShadowMapRenderer->GetShadowMap());
	SetVariable_Scalar(L"gLightIntensity", sceneContext.pLights->GetDirectionalLight().intensity);

	SetVariable_Vector(L"gLightDirection", sceneContext.pLights->GetDirectionalLight().direction);
	SetVariable_Matrix(L"gWorld", pModel->GetTransform()->GetWorld());

	//// get world view projection matrix
	const XMFLOAT4X4 viewProjection{ sceneContext.pCamera->GetViewProjection() };
	XMFLOAT4X4 worldViewProjection;
	XMStoreFloat4x4(&worldViewProjection, XMMatrixMultiply(XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()), XMLoadFloat4x4(&viewProjection)));
	SetVariable_Matrix(L"gWorldViewProj", worldViewProjection);


	//// update view inverse
	const XMFLOAT4X4 viewInverse{ sceneContext.pCamera->GetViewInverse() };
	SetVariable_Matrix(L"gViewInverse", viewInverse);
}

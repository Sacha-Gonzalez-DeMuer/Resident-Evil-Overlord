#include "stdafx.h"
#include "BasicMaterial_Deferred.h"

BasicMaterial_Deferred::BasicMaterial_Deferred() :
	Material(L"Effects/Deferred/BasicEffect_Deferred.fx")
{
}

void BasicMaterial_Deferred::SetDiffuseMap(const std::wstring& assetFile)
{
	SetDiffuseMap(ContentManager::Load<TextureData>(assetFile));
}

void BasicMaterial_Deferred::SetDiffuseMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseDiffuseMap", pTextureData != nullptr);
	SetVariable_Texture(L"gDiffuseMap", pTextureData);
}

void BasicMaterial_Deferred::SetNormalMap(const std::wstring& assetFile)
{
	SetNormalMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred::SetNormalMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseNormalMap", pTextureData != nullptr);
	SetVariable_Texture(L"gNormalMap", pTextureData);
}

void BasicMaterial_Deferred::SetShadowMap(TextureData* pTextureData)
{
	SetVariable_Texture(L"gShadowMap", pTextureData);
}

void BasicMaterial_Deferred::SetSpecularMap(const std::wstring& assetFile)
{
	SetSpecularMap(ContentManager::Load<TextureData>(assetFile));

}

void BasicMaterial_Deferred::SetSpecularMap(TextureData* pTextureData)
{
	SetVariable_Scalar(L"gUseSpecularMap", pTextureData != nullptr);
	SetVariable_Texture(L"gSpecularMap", pTextureData);
}

void BasicMaterial_Deferred::SetSpecularColor(const XMFLOAT4& color)
{
	SetVariable_Vector(L"gSpecularColor", color);
}

void BasicMaterial_Deferred::UseTransparency(bool /*enable*/)
{
	//SetTechnique(enable ? L"Default_Alpha" : L"Default");
}

void BasicMaterial_Deferred::InitializeEffectVariables()
{
}

void BasicMaterial_Deferred::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
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

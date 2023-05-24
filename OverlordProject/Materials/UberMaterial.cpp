#include "stdafx.h"
#include "UberMaterial.h"
#include "FilePaths.h"


UberMaterial::UberMaterial()
	: Material(FilePath::UBERSHADER_FX)
{
}

void UberMaterial::InitializeEffectVariables()
{
	SetVariable_Vector(L"gLightDirection", XMFLOAT3{ -.6f, -.6f, .6f });
	SetVariable_Scalar(L"gUseTextureDiffuse", true);
	SetVariable_Scalar(L"gUseTextureNormal", true);
	SetVariable_Scalar(L"gUseTextureSpecularIntensity", true);
	SetVariable_Vector(L"gColorDiffuse", XMFLOAT3{ Colors::Red });
	SetVariable_Vector(L"gColorSpecular", XMFLOAT3{ Colors::White });
	SetVariable_Scalar(L"gShininess", 8);
	SetVariable_Scalar(L"gFlipGreenChannel", true);
	SetVariable_Scalar(L"gUseFresnelFalloff", true);
	SetVariable_Scalar(L"gFresnelPower", 4);
	SetVariable_Scalar(L"gFresnelMultiplier", 1.5f);
}

void UberMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	if (m_pDiffuseTexture)
		SetVariable_Texture(L"gTextureDiffuse", m_pDiffuseTexture);
}

void UberMaterial::SetHeightMapTexture(const std::wstring& assetFile)
{
	m_pHeightMapTexture = ContentManager::Load<TextureData>(assetFile);

	if (m_pHeightMapTexture)
		SetVariable_Texture(L"gTextureHeight", m_pHeightMapTexture);
}

void UberMaterial::SetNormalMapTexture(const std::wstring& assetFile)
{
	m_pNormalMapTexture = ContentManager::Load<TextureData>(assetFile);

	if (m_pNormalMapTexture)
		SetVariable_Texture(L"gTextureNormal", m_pNormalMapTexture);
}

void UberMaterial::SetSpecularTexture(const std::wstring& assetFile)
{
	m_pSpecularTexture = ContentManager::Load<TextureData>(assetFile);

	if (m_pSpecularTexture)
		SetVariable_Texture(L"gTextureSpecularIntensity", m_pSpecularTexture);
}

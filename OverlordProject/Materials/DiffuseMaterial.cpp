#include "stdafx.h"
#include "DiffuseMaterial.h"
#include "FilePaths.h"

DiffuseMaterial::DiffuseMaterial()
	:Material(FilePath::POS_NORM_TEX_FX)
{
}

void DiffuseMaterial::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);

	if (m_pDiffuseTexture)
		SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial::InitializeEffectVariables()
{

}


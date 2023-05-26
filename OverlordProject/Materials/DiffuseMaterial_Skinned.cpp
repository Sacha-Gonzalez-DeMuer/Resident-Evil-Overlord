#include "stdafx.h"
#include "DiffuseMaterial_Skinned.h"

DiffuseMaterial_Skinned::DiffuseMaterial_Skinned():
	Material(L"Effects/PosNormTex3D_Skinned.fx")
{
}

void DiffuseMaterial_Skinned::SetDiffuseTexture(const std::wstring& assetFile)
{
	m_pDiffuseTexture = ContentManager::Load<TextureData>(assetFile);
	SetVariable_Texture(L"gDiffuseMap", m_pDiffuseTexture);
}

void DiffuseMaterial_Skinned::InitializeEffectVariables()
{
}

void DiffuseMaterial_Skinned::OnUpdateModelVariables(const SceneContext& sceneContext, const ModelComponent* pModel) const
{
	//Retrieve The Animator from the ModelComponent
	//Make sure the animator is not NULL (ASSERT_NULL_)
	//Retrieve the BoneTransforms from the Animator
	//Set the 'gBones' variable of the effect (MatrixArray) > BoneTransforms

     // retrieve animator from the modelcomponent
    const ModelAnimator* pAnimator = pModel->GetAnimator();

    // make sure the animator is not null
    if (!pAnimator)
    {
        Logger::LogError(L"DiffuseMaterial_Skinned::OnUpdateModelVariables > ModelAnimator is null!");
        return;
    }

    // retrieve bone transforms from the animator
    auto boneTransforms = pAnimator->GetBoneTransforms();

    // set the 'gBones' variable of the effect (MatrixArray) to boneTransforms
    SetVariable_MatrixArray(L"gBones", reinterpret_cast<float*>(boneTransforms.data()), static_cast<UINT>(boneTransforms.size()));
    SetVariable_Scalar(L"gNumBones", static_cast<float>(boneTransforms.size()));

    // get world view projection matrix
    const XMFLOAT4X4 viewProjection{ sceneContext.pCamera->GetViewProjection() };
    XMFLOAT4X4 worldViewProjection;
    XMStoreFloat4x4(&worldViewProjection, 
    XMMatrixMultiply(XMLoadFloat4x4(&pModel->GetTransform()->GetWorld()), XMLoadFloat4x4(&viewProjection)));

    // set the 'gWorldViewProj' variable of the effect (Matrix)
    SetVariable_Matrix(L"gWorldViewProj", worldViewProjection);


    // set the 'gWorld' variable of the effect (Matrix)
    SetVariable_Matrix(L"gWorld", pModel->GetTransform()->GetWorld());

}
#pragma once
#include <fbxsdk.h>
namespace dae
{
	struct FBXSceneData;
	class FbxLoader final : public GameObject
	{
	public:
		FbxLoader(const char* file);
		~FbxLoader();

		constexpr bool isValid() const { return m_pFbxManager && m_pFbxScene; }

		void GetScene(FbxNode* root, FBXSceneData& data) ;
		
		std::vector<FbxSurfaceMaterial*> ExtractAllMaterials(FbxNode* root) const;
		std::vector<FbxSurfaceMaterial*> ExtractMaterials(FbxMesh* mesh) const;
		std::vector<FbxLight*> ExtractLights(FbxNode* root) const;
		std::vector<FbxMesh*> ExtractMeshes(FbxNode* node) const;

		// loads fbx model materials into the ovm meshfilter, adds lights and cameras to the scene
		void LoadToOverlord(GameObject& root, SceneContext& sceneContext, const std::wstring& fbxFolderPath);
		void LoadLightsToOverlord(SceneContext& sceneContext);
		void SetModelOffset(const XMFLOAT3& offset) { m_ModelOffset = offset; }
	private:
		bool InitializeFBX();
		void LoadFbxFile(const char* file);

		std::wstring ExtractDiffuseMap(FbxSurfaceMaterial* pMaterial);
		std::wstring ExtractNormalMap(FbxSurfaceMaterial* pMaterial);
		FbxScene* GetFbxScene() const { return m_pFbxScene; }

		Light ConvertToOverlord(FbxLight* fbxLightNode);
		//CameraComponent* ConvertToOverlord(const FbxCamera& fbxNode, GameScene* cameraRoot);
		void MaterialToOverlord_Forward(const std::vector<FbxSurfaceMaterial*>& materials, ModelComponent& toModel, const std::wstring& fbxFolderPath);
		void MaterialToOverlord_Deferred(const std::vector<FbxSurfaceMaterial*>& materials, ModelComponent& toModel, const std::wstring& fbxFolderPath);
	
		void AddLightsToOverlord(const std::vector<FbxLight*>& lights, SceneContext& sceneContext);
		void AddCamerasToOverlord(const std::vector<FbxCamera*>& cameras, SceneContext& sceneContext);

		FbxManager* m_pFbxManager;
		FbxScene* m_pFbxScene;

		XMFLOAT3 m_ModelOffset{};
	};
}
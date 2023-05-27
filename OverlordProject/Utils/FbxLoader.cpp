#include "stdafx.h"
#include "FbxLoader.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/Deferred/BasicMaterial_Deferred.h"
#include "Materials/Deferred/BasicMaterial_Deferred_Shadow.h"
#include "Materials/Deferred/BasicMaterial.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include <iostream>
#include "ResidentEvil/Camera/ReCamera.h"
#include "ResidentEvil/Camera/ReCameraManager.h"

//TODO: remove this
#pragma warning (disable: 4129)
#if _DEBUG
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Debug\\libfbxsdk-md.lib")
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Debug\\libxml2-md.lib")
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Debug\\zlib-md.lib")
#else				  				
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Release\\libfbxsdk-md.lib")
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Release\\libxml2-md.lib")
#pragma comment(lib, "D:\\DAE_SEM4\\Graphics Prog 2\\OverlordEngine\\3rdParty\\FBX SDK\\2020.0.1\\lib\\vs2017\\x64\\Release\\zlib-md.lib")
#endif


// helpful links
// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation
// https://www.youtube.com/watch?v=xLwJFtEc2Ws&ab_channel=%EA%B6%8C%ED%95%98%EB%8A%98
// http://docs.autodesk.com/FBX/2014/ENU/FBX-SDK-Documentation
// https://youtu.be/oIKnBVP2Jgg


namespace dae
{

	struct FBXSceneData
	{
		std::vector<FbxSurfaceMaterial*> fbxMaterials;
		std::vector<FbxLight*> lights;
		std::vector<FbxCamera*> cameras;
		std::vector<FbxMesh*> cameraSwitches;
	};

	FbxLoader::FbxLoader(const char* file)
		: m_pFbxManager(nullptr)
		, m_pFbxScene(nullptr)
	{
		if (InitializeFBX())
		{
			// file to const char*
			LoadFbxFile(file);
			if (!isValid()) throw std::exception("FbxContext::FbxContext() > Failed to load fbx file");
		}
	}

	FbxLoader::~FbxLoader()
	{
		m_pFbxManager->Destroy(); //TODO: fbxmanager leaks anyway? 
		
		//m_pFbxScene->Destroy();
	}

	// converts loaded fbx scene to overlord scene (scene loaded in constructor)
	void FbxLoader::LoadToOverlord(GameObject& root, SceneContext& sceneContext, const std::wstring& fbxFolderPath)
	{
		// find relevant files
		std::wstring fbxFilePath;
		std::wstring ovmFilePath;

		std::wstring searchPath = ContentManager::GetFullAssetPath(fbxFolderPath);
		for (const auto& entry : std::filesystem::directory_iterator(searchPath))
		{
			if (entry.path().extension() == L".fbx")
				fbxFilePath = entry.path().wstring();
			else if (entry.path().extension() == L".ovm")
				ovmFilePath = entry.path().wstring();
		}

		if (fbxFilePath == L"" || ovmFilePath == L"")
			throw std::runtime_error("StaticMeshFactory::LoadModel_MtlObj >> Could not find all required files");
		auto ovmModel = root.AddComponent(new ModelComponent(ovmFilePath));

		// load fbx scene data
		FBXSceneData scene;
		GetScene(m_pFbxScene->GetRootNode(), scene);

		// failed to support cameras :(
		//for (const auto& cam : scene.cameras)
		//	ConvertToOverlord(*cam, root.GetScene());

		AddLightsToOverlord(scene.lights, sceneContext);
		sceneContext.useDeferredRendering
			? MaterialToOverlord_Deferred(scene.fbxMaterials, *ovmModel, fbxFolderPath)
			: MaterialToOverlord_Forward(scene.fbxMaterials, *ovmModel, fbxFolderPath);
	}

	void FbxLoader::LoadLightsToOverlord(SceneContext& sceneContext)
	{
		FBXSceneData scene;
		GetScene(m_pFbxScene->GetRootNode(), scene);
		AddLightsToOverlord(scene.lights, sceneContext);
	}

	void AddOffset(XMFLOAT4& toPos, XMFLOAT3& offset)// TODO: remove this function
	{
		toPos.x += offset.x;
		toPos.y += offset.y;
		toPos.z += offset.z;
	}

	void AddOffset(XMFLOAT3& toPos, XMFLOAT3& offset) // TODO: remove this function
	{
		toPos.x += offset.x;
		toPos.y += offset.y;
		toPos.z += offset.z;
	}

	// (0,90,0) in overlord = (90, 0,0) in blender, so we need to rotate the model, probably my settings that are messed up? i don't know blender
	XMFLOAT3 ApplyRotationMatrix(const XMFLOAT3& v, const XMMATRIX& mat) {
		XMVECTOR vec = XMLoadFloat3(&v);
		vec = XMVector3Transform(vec, mat);
		XMFLOAT3 result;
		XMStoreFloat3(&result, vec);
		return result;
	}

	// Returns a rotation matrix to align two vectors sourceVector and targetVector
	XMMATRIX GetRotationMatrixToAlignVectors(FXMVECTOR sourceVector, FXMVECTOR targetVector)
	{
		// Compute the rotation matrix GG based on the formula GG = [dot(sourceVector,targetVector)-norm(cross(sourceVector,targetVector)) 0; norm(cross(sourceVector,targetVector)) dot(sourceVector,targetVector) 0; 0 0 1]
		float dotProduct = XMVectorGetX(XMVector3Dot(sourceVector, targetVector));
		XMVECTOR crossProduct = XMVector3Cross(sourceVector, targetVector);
		float crossProductNorm = XMVectorGetX(XMVector3Length(crossProduct));
		XMMATRIX GG = XMMATRIX(
			dotProduct - crossProductNorm, 0.0f, 0.0f, 0.0f,
			crossProductNorm, dotProduct, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		// Compute the matrix FFi based on the formula FFi = [sourceVector (targetVector-dot(sourceVector,targetVector)*sourceVector)/norm(targetVector-dot(sourceVector,targetVector)*sourceVector) cross(targetVector,sourceVector)]
		XMVECTOR targetMinusProjection = targetVector - dotProduct * sourceVector;
		XMVECTOR normalizedTargetMinusProjection = XMVector3Normalize(targetMinusProjection);
		XMVECTOR crossProductTargetSource = XMVector3Cross(targetVector, sourceVector);
		XMMATRIX FFi = XMMATRIX(
			XMVectorGetX(sourceVector), XMVectorGetX(normalizedTargetMinusProjection), XMVectorGetX(crossProductTargetSource), 0.0f,
			XMVectorGetY(sourceVector), XMVectorGetY(normalizedTargetMinusProjection), XMVectorGetY(crossProductTargetSource), 0.0f,
			XMVectorGetZ(sourceVector), XMVectorGetZ(normalizedTargetMinusProjection), XMVectorGetZ(crossProductTargetSource), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		// Compute the matrix UU based on the formula UU = FFi * GG * inv(FFi)
		XMMATRIX GGInverse = XMMatrixInverse(nullptr, GG);
		XMMATRIX UU = FFi * GG * GGInverse;

		return UU;
	}


	/* coordinate system A contains a rotation vector, V1 which is defined by (x=90,y=0,z=0)
coordinate system B contains a rotation vector, V2 which is defined by (x=0,y=90,z=0)
what is the rotation matrix to convert from vector V1 to V2*/


	Light FbxLoader::ConvertToOverlord(const FbxLight& fbxLight)
	{
		Light overlordLight{};
		FbxNode* pLightNode = fbxLight.GetNode();

		switch (fbxLight.LightType)
		{
		case FbxLight::eDirectional:
			break; // TODO: support directional light loading

		case FbxLight::ePoint:
		{
			overlordLight.type = LightType::Point;
			break;
		}
		case FbxLight::eSpot:
		{
			overlordLight.type = LightType::Spot;
			break;
		}
		default:
			throw std::runtime_error("FbxLoader::ConvertToOverlord >> Unsupported light type");
		}

		// Extract the position and rotation of the light node
		FbxDouble3 lightPosition = pLightNode->LclTranslation.Get();
		FbxDouble3 lightRotation = pLightNode->LclRotation.Get();

		// Create a light struct and fill in the necessary information
		overlordLight.isEnabled = true;
		overlordLight.position = { static_cast<float>(lightPosition[0]), static_cast<float>(lightPosition[1]), static_cast<float>(lightPosition[2]), 1.0f };
		overlordLight.color = { static_cast<float>(fbxLight.Color.Get()[0]), static_cast<float>(fbxLight.Color.Get()[1]), static_cast<float>(fbxLight.Color.Get()[2]), 1.0f };
		overlordLight.intensity = static_cast<float>(fbxLight.Intensity) * .000001f;

		const auto& prop = fbxLight.FindProperty("Range");
		overlordLight.range = static_cast<float>(prop.Get<FbxDouble>()) * 2;

		pLightNode->Destroy();

		return overlordLight;
	}


	//CameraComponent* FbxLoader::ConvertToOverlord(const FbxCamera& fbxCam, GameScene* cameraRoot)
	//{
	//	// Extract the position and rotation of the camera noder
	//	//const FbxDouble3& cameraPosition = fbxCam.GetNode()->LclTranslation.Get();
	//	const FbxDouble3& cameraRotation = fbxCam.GetNode()->LclRotation.Get();
	//	const auto& pos = fbxCam.EvaluatePosition();
	//	const XMFLOAT3& camPos = { static_cast<float>(pos[0]), -static_cast<float>(pos[2]), static_cast<float>(pos[1]) };
	//	const XMFLOAT3& rotation = { static_cast<float>(cameraRotation[0]), static_cast<float>(cameraRotation[1]), static_cast<float>(cameraRotation[2]) };
	//	auto camVolume = new CameraVolume(camPos, { 1,1 }, rotation);
	//	camVolume->GetRoll() = static_cast<float>(fbxCam.Roll.Get());
	//	cameraRoot->AddChild(camVolume);
	//	CameraVolumeManager::Get().AddVolume(camVolume);
	//	//auto pCamera = camGO->AddComponent(new CameraComponent());
	//	//camGO->GetTransform()->Rotate(rotation, false);
	//	//camGO->GetTransform()->Translate(camPos.x, camPos.y, camPos.z);
	//	//pCamera->SetActive(true);
	//	return nullptr;
	//}

	// applies fbx materials to ovm model component
	void FbxLoader::MaterialToOverlord_Forward(const std::vector<FbxSurfaceMaterial*>& materials, ModelComponent& toModel, const std::wstring& fbxFolderPath)
	{
		// build materials
		std::vector<DiffuseMaterial_Shadow*> diffuseMaterials{};
		for (const auto& mat : materials)
		{
			const auto& diffuseMap = ExtractDiffuseMap(mat);
			if (diffuseMap == L"") continue;

			auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();

			pDiffuseMat->SetDiffuseTexture(fbxFolderPath + diffuseMap);
			diffuseMaterials.push_back(pDiffuseMat);
		}

		for (UINT8 i = 0; i < diffuseMaterials.size(); ++i)
			toModel.SetMaterial(diffuseMaterials[i], i);
	}

	void FbxLoader::MaterialToOverlord_Deferred(const std::vector<FbxSurfaceMaterial*>& materials, ModelComponent& toModel, const std::wstring& fbxFolderPath)
	{
		// build materials
		std::vector<BasicMaterial_Deferred_Shadow*> basicMaterials{};
		for (const auto& mat : materials)
		{
			std::wstring diffuseMap = ExtractDiffuseMap(mat);
			if (diffuseMap == L"") continue;

			auto pBasicMat = MaterialManager::Get()->CreateMaterial<BasicMaterial_Deferred_Shadow>();
			pBasicMat->UseTransparency(false);

			pBasicMat->SetDiffuseMap(fbxFolderPath + diffuseMap);
			basicMaterials.push_back(pBasicMat);
		}

		for (UINT8 i = 0; i < basicMaterials.size(); ++i)
			toModel.SetMaterial(basicMaterials[i], i);
	}

	void FbxLoader::AddLightsToOverlord(const std::vector<FbxLight*>& lights, SceneContext& sceneContext)
	{
		for (const auto& light : lights)
		{
			auto overlordLight = ConvertToOverlord(*light);
			sceneContext.pLights->AddLight(overlordLight);
		}
	}

	void FbxLoader::AddCamerasToOverlord(const std::vector<FbxCamera*>& /*cameras*/, SceneContext& /*sceneContext*/)
	{

	}

	std::vector<FbxSurfaceMaterial*> FbxLoader::ExtractAllMaterials(FbxNode* root) const
	{
		std::vector<FbxSurfaceMaterial*> materials{};
		if (root)
		{
			FbxNodeAttribute* attribute = root->GetNodeAttribute();
			if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = root->GetMesh();
				if (mesh)
				{
					int materialCount = mesh->GetNode()->GetMaterialCount();
					for (int i = 0; i < materialCount; i++)
					{
						FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(i);
						if (material)
						{
							// Extract material information here...
							materials.push_back(material);
						}
					}
				}
			}

			for (int i = 0; i < root->GetChildCount(); i++)
			{
				auto childMats = ExtractAllMaterials(root->GetChild(i));

				materials.insert(materials.end(), childMats.begin(), childMats.end());
			}
		}

		return materials;
	}

	std::vector<FbxSurfaceMaterial*> FbxLoader::ExtractMaterials(FbxMesh* mesh) const
	{
		std::vector<FbxSurfaceMaterial*> materials;
		if (mesh)
		{
			int materialCount = mesh->GetNode()->GetMaterialCount();
			for (int i = 0; i < materialCount; i++)
			{
				FbxSurfaceMaterial* material = mesh->GetNode()->GetMaterial(i);
				if (material)
					materials.push_back(material);
			}
		}

		return materials;
	}

	std::vector<FbxMesh*> FbxLoader::ExtractMeshes(FbxNode* node) const
	{
		std::vector<FbxMesh*> meshes{};

		if (node)
		{
			FbxNodeAttribute* attribute = node->GetNodeAttribute();
			if (attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				FbxMesh* mesh = node->GetMesh();
				if (mesh)
					meshes.push_back(mesh);
			}
		}

		// recursively process child nodes
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			auto childMeshes = ExtractMeshes(node->GetChild(i));
			meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
		}

		return meshes;
	}

	std::wstring FbxLoader::ExtractDiffuseMap(FbxSurfaceMaterial* pMaterial)
	{
		// Get the diffuse texture, if any.
		FbxProperty pDiffuseProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (pDiffuseProperty.IsValid())
		{
			int nTextureCount = pDiffuseProperty.GetSrcObjectCount<FbxTexture>();
			if (nTextureCount > 0)
			{
				FbxFileTexture* pDiffuseTexture = pDiffuseProperty.GetSrcObject<FbxFileTexture>(0);
				const char* pDiffuseFileName = pDiffuseTexture->GetMediaName();

				return std::wstring(pDiffuseFileName, pDiffuseFileName + strlen(pDiffuseFileName));
			}
		}

		return std::wstring();
	}

	std::wstring FbxLoader::ExtractNormalMap(FbxSurfaceMaterial* pMaterial)
	{
		// Get the normal texture, if any.
		FbxProperty pNormalProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (pNormalProperty.IsValid())
		{
			int nTextureCount = pNormalProperty.GetSrcObjectCount<FbxTexture>();
			if (nTextureCount > 0)
			{
				FbxFileTexture* pNormalTexture = pNormalProperty.GetSrcObject<FbxFileTexture>(0);
				const char* pNormalFileName = pNormalTexture->GetMediaName();
				return std::wstring(pNormalFileName, pNormalFileName + strlen(pNormalFileName));
			}
		}

		return std::wstring();
	}

	bool FbxLoader::InitializeFBX()
	{
		//1. Create an instance of the FBX SDK manager
		m_pFbxManager = FbxManager::Create();

		if (!m_pFbxManager) return false;

		//2. set import settings
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		pIOsettings->SetBoolProp(IMP_FBX_MATERIAL, true);
		pIOsettings->SetBoolProp(IMP_FBX_TEXTURE, true);
		pIOsettings->SetBoolProp(IMP_CAMERA, true);
		pIOsettings->SetBoolProp(IMP_LIGHT, true);
		m_pFbxManager->SetIOSettings(pIOsettings);
			

		return true;
	}

	void FbxLoader::LoadFbxFile(const char* file)
	{
		//3. Create an FBX importer object and initialize it
		FbxImporter* pImporter = FbxImporter::Create(m_pFbxManager, "Importer");
		if (!pImporter->Initialize(file, -1, m_pFbxManager->GetIOSettings()))
			return;

		// 4. Create a scene object and import the FBX file into it
		m_pFbxScene = FbxScene::Create(m_pFbxManager, "new_scene");
		pImporter->Import(m_pFbxScene);
		pImporter->Destroy();

		//// 5. Transform fbx to DX11 axis
		//FbxAxisSystem axisSystem{ FbxAxisSystem::eDirectX };
		//axisSystem.ConvertScene(m_pFbxScene);
	}


	void FbxLoader::GetScene(FbxNode* root, FBXSceneData& data)
	{
		if (!root)
		{
			root = m_pFbxScene->GetRootNode();
			if (!root) throw std::runtime_error("No root node found in scene.");
		}

		// 5. Traverse the scene and extract the necessary data.
		const auto childCount = root->GetChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* child = root->GetChild(i);
			if (!child) continue;

			FbxNodeAttribute* attribute = child->GetNodeAttribute();
			if (attribute)
			{
				FbxNodeAttribute::EType attributeType = attribute->GetAttributeType();
				switch (attributeType)
				{
				case FbxNodeAttribute::eMesh: // save materials from mesh to apply later
				{
					FbxMesh* mesh = child->GetMesh();
					auto mats = ExtractMaterials(mesh);
					data.fbxMaterials.insert(data.fbxMaterials.end(), mats.begin(), mats.end());

					auto tag = mesh->FindProperty("Tag").Get<FbxString>();
					if(tag == FbxString("CameraSwitcher"))
						data.cameraSwitches.push_back(mesh);

					break;
				}
				case FbxNodeAttribute::eLight:
				{
					data.lights.push_back(child->GetLight());
					break;
				}
				case FbxNodeAttribute::eCamera:
				{
					data.cameras.push_back(child->GetCamera());
					break;
				}
				}
			}
		}
	}
}
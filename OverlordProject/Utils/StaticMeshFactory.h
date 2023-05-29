#pragma once
#include "Materials/DiffuseMaterial.h"

// class to load .obj and .mtl files into overlord
class StaticMeshFactory final : public Singleton<StaticMeshFactory>
{
public:
	// holds name of submesh and name of its material
	using SubmeshMaterialInfoVector = std::vector<std::pair<std::wstring, std::wstring>>;
public:
	StaticMeshFactory() = default;
	~StaticMeshFactory() = default;
	StaticMeshFactory(const StaticMeshFactory& other) = delete;
	StaticMeshFactory(StaticMeshFactory&& other) noexcept = delete;
	StaticMeshFactory& operator=(const StaticMeshFactory& other) = delete;
	StaticMeshFactory& operator=(StaticMeshFactory&& other) noexcept = delete;

	template<typename T_MaterialType>
	void AddMtlModelComponent(GameObject& modelObj, const std::wstring& folderPath);

	struct MtlMaterial
	{
		std::wstring name;
		DirectX::XMFLOAT3 ambient;
		DirectX::XMFLOAT3 diffuse;
		DirectX::XMFLOAT3 specular;
		float shininess;
		std::wstring diffuseFile;

		MtlMaterial() :
			name{},
			ambient{ 0.0f, 0.0f, 0.0f },
			diffuse{ 0.0f, 0.0f, 0.0f },
			specular{ 0.0f, 0.0f, 0.0f },
			shininess{ 0.0f },
			diffuseFile{}
		{}

		MtlMaterial(const std::wstring& name, const DirectX::XMFLOAT3& ambient, const DirectX::XMFLOAT3& diffuse, const DirectX::XMFLOAT3& specular, float shininess, const std::wstring& diffuseFile) :
			name{ name },
			ambient{ ambient },
			diffuse{ diffuse },
			specular{ specular },
			shininess{ shininess },
			diffuseFile{ diffuseFile }
		{}
	};

	std::vector<MtlMaterial> ParseMTL(const std::wstring& mtlFilePath);

	struct Object3D {
		std::wstring name;
		std::wstring materialName;
	};

	std::vector<Object3D> GetMeshNamesAndMaterials(const std::wstring& objFilePath);

};

template<typename T_MaterialType>
inline void StaticMeshFactory::AddMtlModelComponent(GameObject& modelObj, const std::wstring& folderPath)
{
	// check if template is of class material
	static_assert(std::is_base_of<Material<T_MaterialType>, T_MaterialType>::value, "StaticMeshFactory::AddMtlModelComponent >> T_MaterialType must be of type Material");
	// find relevant files
	std::wstring objFilePath;
	std::wstring mtlFilePath;
	std::wstring ovmFilePath;

	//std::wstring searchPath = ContentManager::GetFullAssetPath(folderPath);
	for (const auto& entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.path().extension() == L".obj")
			objFilePath = entry.path().wstring();
		else if (entry.path().extension() == L".mtl")
			mtlFilePath = entry.path().wstring();
		else if (entry.path().extension() == L".ovm")
			ovmFilePath = entry.path().wstring();
	}

	if (objFilePath == L"" || mtlFilePath == L"" || ovmFilePath == L"")
		throw std::runtime_error("StaticMeshFactory::LoadModel_MtlObj >> Could not find all required files");

	auto model = modelObj.AddComponent(new ModelComponent(ovmFilePath));
	auto materialInfo = ParseMTL(mtlFilePath);
	auto submeshInfo = GetMeshNamesAndMaterials(objFilePath);



	std::vector<T_MaterialType*> materials{ };
	materials.reserve(materialInfo.size());
	std::map<std::wstring, T_MaterialType*> materialMap;

	for (const auto& submesh : submeshInfo)
	{
		const auto matName = submesh.materialName;

		// check if material with same name has already been added
		const auto& existingMat = materialMap.find(matName);
		if (existingMat != materialMap.end())
		{
			continue;
		}

		// find corresponding material info
		const auto& it = std::find_if(materialInfo.begin(), materialInfo.end(), [&matName](const MtlMaterial& mat) {return mat.name == matName; });

		if (it == materialInfo.end())
			throw std::runtime_error("StaticMeshFactory::LoadModel_MtlObj >> Could not find material info");


		auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<T_MaterialType>();

		auto path = folderPath + it->diffuseFile;
		if (!std::filesystem::exists(ContentManager::GetFullAssetPath(path)))
			continue;

		pDiffuseMat->SetDiffuseTexture(folderPath + it->diffuseFile);
		materials.emplace_back(pDiffuseMat);
	}

	for (UINT8 i = 0; i < materials.size(); ++i)
		model->SetMaterial(materials[i], i);
}

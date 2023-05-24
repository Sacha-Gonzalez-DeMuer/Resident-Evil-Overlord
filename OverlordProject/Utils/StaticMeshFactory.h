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


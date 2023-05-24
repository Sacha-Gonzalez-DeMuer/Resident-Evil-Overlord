#include "stdafx.h"
#include "StaticMeshFactory.h"
#include "../OverlordEngine/Components/ModelComponent.h"
#include <map>

void StaticMeshFactory::AddMtlModelComponent(GameObject& modelObj, const std::wstring& folderPath)
{
    // find relevant files
    std::wstring objFilePath;
    std::wstring mtlFilePath;
    std::wstring ovmFilePath;

    std::wstring searchPath = ContentManager::GetFullAssetPath(folderPath);
    for (const auto& entry : std::filesystem::directory_iterator(searchPath))
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

    std::vector<DiffuseMaterial*> materials{ };
    materials.reserve(materialInfo.size());
    std::map<std::wstring, DiffuseMaterial*> materialMap;

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


        auto pDiffuseMat = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();

        auto path = folderPath + it->diffuseFile;
        if (!std::filesystem::exists(ContentManager::GetFullAssetPath(path)))
            continue;
        
        pDiffuseMat->SetDiffuseTexture(folderPath + it->diffuseFile);
        materials.emplace_back(pDiffuseMat);
    }

    for (UINT8 i = 0; i < materials.size(); ++i)
        model->SetMaterial(materials[i], i);
}


std::vector<StaticMeshFactory::MtlMaterial> StaticMeshFactory::ParseMTL(const std::wstring& mtlFilePath)
{
    std::vector<MtlMaterial> materials;

    std::wifstream file(ContentManager::GetFullAssetPath(mtlFilePath));
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    MtlMaterial currentMaterial{};
    std::wstring line;
    while (std::getline(file, line))
    {
        std::wstringstream ss(line);
        std::wstring keyword;
        ss >> keyword;

        if (keyword == L"newmtl")
        {
            if (currentMaterial.name != L"")
            {
                materials.push_back(currentMaterial);
            }
            currentMaterial = MtlMaterial();
            ss >> currentMaterial.name;
        }
        else if (keyword == L"Ka")
        {
            ss >> currentMaterial.ambient.x >> currentMaterial.ambient.y >> currentMaterial.ambient.z;
        }
        else if (keyword == L"Kd")
        {
            ss >> currentMaterial.diffuse.x >> currentMaterial.diffuse.y >> currentMaterial.diffuse.z;
        }
        else if (keyword == L"Ks")
        {
            ss >> currentMaterial.specular.x >> currentMaterial.specular.y >> currentMaterial.specular.z;
        }
        else if (keyword == L"Ns")
        {
            ss >> currentMaterial.shininess;
        }
        else if (keyword == L"map_Kd")
        {
            ss >> currentMaterial.diffuseFile;
        }
    }

    if (currentMaterial.name != L"")
    {
        materials.push_back(currentMaterial);
    }

    return materials;
}

std::vector<StaticMeshFactory::Object3D> StaticMeshFactory::GetMeshNamesAndMaterials(const std::wstring& objFilePath)
{
    std::vector<Object3D> objInfoVec;

    std::wifstream file(objFilePath);
    std::wstring line;
    Object3D objInfo;
    std::wistringstream iss;
    std::wstring token;
    while (std::getline(file, line)) {
        token.clear();
        iss.clear(); // clear the wistringstream before each use
        iss.str(line);

        iss >> token;

        if (token == L"o") { // "o" specifies the mesh name
            if (objInfo.name != L"") { // push the previous object info onto the vector
                objInfoVec.push_back(objInfo);
            }
            objInfo.name = L"";
            objInfo.materialName = L"";
            iss >> objInfo.name;
        }
        else if (token == L"usemtl") { // "usemtl" specifies the material name
            iss >> objInfo.materialName;
        }
    }

    if (objInfo.name != L"") { // push the last object info onto the vector
        objInfoVec.push_back(objInfo);
    }

    return objInfoVec;
}

#pragma once
#include <string>

namespace FilePath
{
	// Effects
	const std::wstring POS_NORM_TEX_FX{ L"Effects/PosNormTex3d.fx" };
	const std::wstring UBERSHADER_FX{ L"Effects/UberShaderFX.fx" };
	const std::wstring SPIKY_FX{ L"Effects/SpikyShader.fx" };

	// Chair
	const std::wstring CHAIR_MESH{ L"Meshes/Chair.ovm" };
	const std::wstring CHAIR_CONVEX_MESH{ L"Meshes/Chair.ovpc" };
	const std::wstring CHAIR_TEX{ L"Textures/Chair_Dark.dds" };

	// Sphere
	const std::wstring SPHERE_MESH{ L"Meshes/Sphere.ovm" };

	// Octa Sphere
	const std::wstring OCTASPHERE_MESH{ L"Meshes/OctaSphere.ovm" };

	// Skull material
	const std::wstring SKULL_DIFFUSE{ L"Textures/Skulls Textures/Skulls_Diffusemap.tga" };
	const std::wstring SKULL_NORMAL{ L"Textures/Skulls Textures/Skulls_Normalmap.tga" };
	const std::wstring SKULL_HEIGHT{ L"Textures/Skulls Textures/Skulls_Heightmap.tga" };

	// Mixamo Soldier
	const std::wstring SOLDIER_BODY_DIFFUSE{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_body1_diffuse.png " };
	const std::wstring SOLDIER_BODY_SPEC{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_body1_specular.png" };
	const std::wstring SOLDIER_BODY_NORMAL{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_body1_normal.png" };
	const std::wstring SOLDIER_HEAD_DIFFUSE{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_head6_diffuse.png " };
	const std::wstring SOLDIER_HEAD_SPEC{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_head6_specular.png" };
	const std::wstring SOLDIER_WALKING_OVM{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_Walking.ovm" };

	// Dining Hall
	const std::wstring FOLDER_ENV_DINING{ L"ResidentEvil/Environments/DiningHall/FBX/" };
	const std::wstring ENV_DINING_FBX{ L"ResidentEvil/Environments/DiningHall/FBX/DiningHall.fbx" };
	const std::wstring ENV_DINING_OVM{ L"ResidentEvil/Environments/DiningHall/FBX/DiningHall.ovm" };
	const std::wstring ENV_DINING_NAVMESH{ L"ResidentEvil/Environments/DiningHall/DiningHall_NavMesh.ovm" };

	// Main Hall
	const std::wstring FOLDER_ENV_MAINHALL{ L"ResidentEvil/Environments/MainHall/" };
}
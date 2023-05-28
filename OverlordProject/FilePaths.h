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
	const std::wstring SOLDIER_ANIMS_OVM{ L"ResidentEvil/Characters/ChrisRedfield/Mixamo/Soldier_AllAnims.ovm" };

	// Dining Hall
	const std::wstring FOLDER_ENV_DINING{ L"ResidentEvil/Environments/DiningHall/FBX/" };
	const std::wstring ENV_DINING_FBX{ L"ResidentEvil/Environments/DiningHall/FBX/DiningHall.fbx" };
	const std::wstring ENV_DINING_OVM{ L"ResidentEvil/Environments/DiningHall/FBX/DiningHall.ovm" };
	const std::wstring ENV_DINING_NAVMESH{ L"ResidentEvil/Environments/DiningHall/DiningHall_NavMesh.ovpt" };

	// Main Hall
	const std::wstring FOLDER_ENV_MAINHALL{ L"ResidentEvil/Environments/MainHall/" };

	// Stairs
	const std::wstring FOLDER_ENV_STAIRS{ L"ResidentEvil/Environments/Stairs/FBX/" };
	const std::wstring ENV_STAIRS_FBX{ L"ResidentEvil/Environments/Stairs/FBX/Stairs.fbx" };
	const std::wstring ENV_STAIRS_OVM{ L"ResidentEvil/Environments/Stairs/FBX/Stairs.ovm" };

	// Zombie
	const std::wstring ZOMBIE_ANIMS_OVM{ L"ResidentEvil/Characters/Zombie/Zombie2.ovm" };
	const std::wstring ZOMBIE_DIFFUSE{ L"ResidentEvil/Characters/Zombie/Zombie_diffuse.png" };
	const std::wstring ZOMBIE_SPEC{ L"ResidentEvil/Characters/Zombie/Zombie_specular.png" };
	const std::wstring ZOMBIE_NORMAL{ L"ResidentEvil/Characters/Zombie/Zombie_normal.png" };

	// HUD
	const std::wstring SUBTITLE_FONT{ L"Fonts/Consolas_32.fnt" };
	const std::wstring TEST_SPRITE{ L"Textures/TestSprite.jpg" };


	// PARTICLES
	const std::wstring BLOOD_PARTICLE{ L"ResidentEvil/Particles/Blood.png" };
}
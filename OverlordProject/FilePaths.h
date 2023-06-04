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
	const std::wstring ENV_MAIN_NAVMESH{ L"ResidentEvil/Environments/MainHall/Additional/MainHall_NavMesh.ovpt" };

	// Stairs
	const std::wstring FOLDER_ENV_STAIRS{ L"ResidentEvil/Environments/Stairs/FBX/" };
	const std::wstring ENV_STAIRS_FBX{ L"ResidentEvil/Environments/Stairs/FBX/Stairs.fbx" };
	const std::wstring ENV_STAIRS_OVM{ L"ResidentEvil/Environments/Stairs/FBX/Stairs.ovm" };

	// Zombie
	const std::wstring ZOMBIE_ANIMS_OVM{ L"ResidentEvil/Characters/Zombie/Zombie.ovm" };
	const std::wstring ZOMBIE_DIFFUSE{ L"ResidentEvil/Characters/Zombie/Zombie_diffuse.png" };
	const std::wstring ZOMBIE_SPEC{ L"ResidentEvil/Characters/Zombie/Zombie_specular.png" };
	const std::wstring ZOMBIE_NORMAL{ L"ResidentEvil/Characters/Zombie/Zombie_normal.png" };

	// HUD
	const std::wstring DEFAULT_FONT{ L"Fonts/Consolas_32.fnt" };
	const std::wstring TEST_SPRITE{ L"Textures/TestSprite.jpg" };

	// PARTICLES
	const std::wstring BLOOD_PARTICLE{ L"ResidentEvil/Particles/Blood.png" };

	// AUDIO
	const std::wstring DINING_AMBIENT_AUDIO{ L"ResidentEvil/Audio/DiningHall_Ambient.wav" };
	const std::wstring MAINHALL_AMBIENT_AUDIO{ L"ResidentEvil/Audio/MainHall_Ambient.wav" };
	const std::wstring MAIN_AMBIENT_AUDIO{ L"ResidentEvil/Audio/Ambient_Main.wav" };
	const std::wstring ZOMBIE_IDLE_AUDIO{L"ResidentEvil/Audio/Zombie_Idle.wav"};
	const std::wstring ZOMBIE_AGGRO_AUDIO{ L"ResidentEvil/Audio/Zombie_Aggro.wav" };
	const std::wstring ZOMBIE_DETECTED_AUDIO{ L"ResidentEvil/Audio/Zombie_Detected.wav" };
	const std::wstring DOOR_AUDIO{ L"ResidentEvil/Audio/Door.mp3" };
	const std::wstring GUNSHOT_AUDIO{ L"ResidentEvil/Audio/Gunshot.wav" };
	const std::wstring FOOTSTEPA_AUDIO{ L"ResidentEvil/Audio/SE_StepA.wav "};
	const std::wstring FOOTSTEPB_AUDIO { L"ResidentEvil/Audio/SE_StepB.wav" };
	const std::wstring BUTTON_CLICK_AUDIO{ L"ResidentEvil/Audio/Confirm.wav" };
	const std::wstring BUTTON_HOVER_AUDIO{ L"ResidentEvil/Audio/Hover.wav" };
	const std::wstring EVIL01_AUDIO{ L"ResidentEvil/Audio/EVIL01.wav" };
	const std::wstring STANDBY_AUDIO{ L"ResidentEvil/Audio/Standby.wav" };


	// Door
	const std::wstring DOOR_FOLDER{ L"ResidentEvil/Props/Door/" };
	const std::wstring DOOR_MESH{ L"ResidentEvil/Props/Door/Door.ovm" };
	const std::wstring DOOR_DIFFUSE{ L"ResidentEvil/Props/Door/DoorDiffuse.png" };
	const std::wstring DOOR_NORMAL{ L"ResidentEvil/Props/Door/DoorNormal.png "};
	const std::wstring DOOR_SPEC{ L"ResidentEvil/Props/Door/DoorSpecular.png "};

	// Text
	const std::wstring SUB_FONT{ L"ResidentEvil/Fonts/ReFont_subtitles.fnt" };

	// Images
	const std::wstring MAINMENU_BACKGROUND_IMG{ L"ResidentEvil/Img/MenuBackground.jpg" };
	const std::wstring MAINMENU_STANDBY_IMG{L"ResidentEvil/Img/Standby.jpg"};
	const std::wstring CONTROLS_IMG{ L"ResidentEvil/Img/Controls.jpg" };
}
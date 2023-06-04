#pragma once
#include <map>
#include "ReData.h"

class ReScene;
class ReGameManager 
{
public:
	static ReGameManager& Get()
	{
		static ReGameManager instance{};
		return instance;
	}

	ReGameManager(const ReGameManager& other) = delete;
	ReGameManager(ReGameManager&& other) noexcept = delete;
	ReGameManager& operator=(const ReGameManager& other) = delete;
	ReGameManager& operator=(ReGameManager&& other) noexcept = delete;
	~ReGameManager() = default;

	// scenes have multiple spawn points depending on where you're coming from in the other scene
	void SetSpawnPos(const XMFLOAT3& spawnPos) { m_SpawnPos = spawnPos; }
	const XMFLOAT3& GetSpawnPos() const { return m_SpawnPos; }

	void LoadResidentEvil();
	void StartScene(ReScenes scene);

	void Exit();

private:
	ReGameManager() = default;
	ReScene* m_CurrentScene;
	XMFLOAT3 m_SpawnPos{};
	std::map<ReScenes, XMFLOAT3> m_SpawnPosMap{};
	std::map<ReScenes, ReSceneData> m_ScenesMap{};
};


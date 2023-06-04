#pragma once
class ReScene : public GameScene
{
public:
	ReScene(const std::wstring& sceneName) : GameScene(sceneName) {};
	virtual ~ReScene(void) {};

	virtual void Start() = 0;
	virtual void Reset() = 0;

protected:
	bool m_WorldLoaded{ false };
};

#pragma once
class ModelTestScene : public GameScene
{
public:
	ModelTestScene() : GameScene{ L"ModelTestScene" }, m_pChair{ nullptr } {};
	~ModelTestScene() override = default;

	ModelTestScene(const ModelTestScene& other) = delete;
	ModelTestScene(ModelTestScene&& other) noexcept = delete;
	ModelTestScene& operator=(const ModelTestScene& other) = delete;
	ModelTestScene& operator=(ModelTestScene&& other) noexcept = delete;

protected:
	void Initialize() override;

private:
	GameObject* m_pChair;
};

#pragma once
class StairsScene final : public GameScene
{
public:
	StairsScene();
	~StairsScene() override = default;
	StairsScene(const StairsScene& other) = delete;
	StairsScene(StairsScene&& other) noexcept = delete;
	StairsScene& operator=(const StairsScene& other) = delete;
	StairsScene& operator=(StairsScene&& other) noexcept = delete;
	void Initialize() override;
private:
};


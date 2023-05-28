#pragma once
class PostGrayscale;
class PostBlur;
class PostAmbientOcclusion;
class PostBloom;
class PostProcessingScene : public GameScene
{
public:
	PostProcessingScene():GameScene(L"PostProcessingScene"){}
	~PostProcessingScene() override = default;
	PostProcessingScene(const PostProcessingScene& other) = delete;
	PostProcessingScene(PostProcessingScene&& other) noexcept = delete;
	PostProcessingScene& operator=(const PostProcessingScene& other) = delete;
	PostProcessingScene& operator=(PostProcessingScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void OnGUI() override;

private:
	PostBlur* m_pPostBlur;
	PostBloom* m_pPostBloom;
	PostAmbientOcclusion* m_pPostAmbientOcclusion;
};


#pragma once
class PostAmbientOcclusion : public PostProcessingMaterial
{
public:
	PostAmbientOcclusion();
	~PostAmbientOcclusion() override = default;
	PostAmbientOcclusion(const PostAmbientOcclusion& other) = delete;
	PostAmbientOcclusion(PostAmbientOcclusion&& other) noexcept = delete;
	PostAmbientOcclusion& operator=(const PostAmbientOcclusion& other) = delete;
	PostAmbientOcclusion& operator=(PostAmbientOcclusion&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {};
};
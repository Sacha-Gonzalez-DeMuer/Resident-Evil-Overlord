#pragma once
class PostGrain final : public PostProcessingMaterial
{
public:
	PostGrain();
	~PostGrain() override = default;
	PostGrain(const PostGrain& other) = delete;
	PostGrain(PostGrain&& other) noexcept = delete;
	PostGrain& operator=(const PostGrain& other) = delete;
	PostGrain& operator=(PostGrain&& other) noexcept = delete;

private:
	void Initialize(const GameContext& ) override {};
};



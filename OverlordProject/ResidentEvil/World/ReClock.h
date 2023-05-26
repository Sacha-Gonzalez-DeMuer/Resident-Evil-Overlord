#pragma once
#include "ResidentEvil/World/ReInteractableObject.h"
#include "ResidentEvil/ReData.h"

class ReClock final : public ReInteractableObject
{
public:
	ReClock(const XMFLOAT3& pos, const XMFLOAT3& size);
	~ReClock() override = default;
	ReClock(const ReClock& other) = delete;
	ReClock(ReClock&& other) noexcept = delete;
	ReClock& operator=(const ReClock& other) = delete;
	ReClock& operator=(ReClock&& other) noexcept = delete;

	void Interact() override;
	void SetSubtitle(const Subtitle& subtitle) { m_Subtitle = subtitle; }

private:
	Subtitle m_Subtitle;
};


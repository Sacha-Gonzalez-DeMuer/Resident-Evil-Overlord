#include "stdafx.h"
#include "ModelAnimator.h"
#include <vector>
ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	m_AnimationClips = m_pMeshFilter->GetAnimationClips();
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		// Calculate the passedTicks
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;
		
		passedTicks = fmod(passedTicks, m_CurrentClip.duration);

		// Add the passedTicks to tick count
		if (m_Reversed)
		{
			m_TickCount -= passedTicks;
			if (m_TickCount < 0)
				m_TickCount = m_CurrentClip.duration;
		}
		else
		{
			m_TickCount += passedTicks;
			if (m_TickCount > m_CurrentClip.duration) {
				if (m_SinglePlay)
				{
					m_SinglePlay = false;
					if (m_OnCompletionCallback)
					{
						m_OnCompletionCallback();
						m_OnCompletionCallback = nullptr;
					}
					if (m_HasNext) SetAnimation(m_NextClip);
				}

				m_TickCount = 0;
			}
		}

		// Determine affected Keys based on m_TickCount
		AnimationKey keyA, keyB;
		for (const auto& key : m_CurrentClip.keys)
		{
			if (key.tick < m_TickCount)
			{
				keyA = key;
			}
			else
			{
				keyB = key;
				break;
			}
		}

		// Interpolate between keys
		auto blendFactor = (m_TickCount - keyA.tick) / (keyB.tick - keyA.tick);
		m_Transforms.resize(keyA.boneTransforms.size());

		for (size_t i = 0; i < keyA.boneTransforms.size(); ++i)
		{
			// Load matrices into XMMATRIX variables
			XMMATRIX matA = XMLoadFloat4x4(&keyA.boneTransforms[i]);
			XMMATRIX matB = XMLoadFloat4x4(&keyB.boneTransforms[i]);

			// Decompose both transforms
			XMVECTOR posA, scaleA, posB, scaleB;
			XMVECTOR rotA, rotB;
			XMMatrixDecompose(&scaleA, &rotA, &posA, matA);
			XMMatrixDecompose(&scaleB, &rotB, &posB, matB);

			// Lerp between all the transformations (Position, Scale, Rotation)
			XMVECTOR posLerp = XMVectorLerp(posA, posB, blendFactor);
			XMVECTOR scaleLerp = XMVectorLerp(scaleA, scaleB, blendFactor);
			XMVECTOR rotLerp = XMQuaternionSlerp(rotA, rotB, blendFactor);

			// Compose a transformation matrix with the lerp-results
			XMMATRIX matLerp = XMMatrixAffineTransformation(scaleLerp, XMVectorZero(), rotLerp, posLerp);

			// Store the resulting matrix
			XMStoreFloat4x4(&m_Transforms[i], matLerp);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (const auto& animClip : m_AnimationClips)
	{
		if (animClip.name == clipName)
		{
			SetAnimation(animClip);
			return;
		}
	}

	//	Call Reset
	//	Log a warning with an appropriate message
	Reset(false);	
	Logger::LogWarning(L"ModelAnimator::SetAnimation(const std::wstring& clipName) > clipName is not found in the m_AnimationClips vector");
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < m_AnimationClips.size())
	{
		if (m_AnimationClips.empty())
		{
			Logger::LogWarning(L"ModelAnimator::SetAnimation(UINT clipNumber) > m_AnimationClips is empty");
			return;
		}

		//	Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		const auto& clip = m_AnimationClips[clipNumber];

		//	Call SetAnimation(AnimationClip clip)
		SetAnimation(clip);
	}
	else
	{

		//	Call Reset
		Reset(false);

		//	Log a warning with an appropriate message
		Logger::LogWarning(L"ModelAnimator::SetAnimation(UINT clipNumber) > clipNumber is bigger than the actual m_AnimationClips vector size");
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	m_ClipSet = true;
	m_CurrentClip = clip;
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	m_IsPlaying = !pause;

	//Set m_TickCount to zero
	//Set m_AnimationSpeed to 1.0f
	m_TickCount = 0;
	m_AnimationSpeed = 1.0f;

	if (m_ClipSet)
	{
		//	Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		const auto& boneTransforms = m_CurrentClip.keys[0].boneTransforms;

		//	Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms.assign(boneTransforms.begin(), boneTransforms.end());
	}
	else
	{
		// create identity matrix
		XMFLOAT4X4 matrix;
		XMStoreFloat4x4(&matrix, XMMatrixIdentity());

		// refilll m_transforms with this identitymatrix
		m_Transforms.assign(m_pMeshFilter->m_BoneCount, matrix);
	}
}

void ModelAnimator::PlayOnce(const std::wstring& clipName, const std::wstring& nextClipName)
{
	m_Reversed = false;
	m_IsPlaying = true;
	m_SinglePlay = true;
	SetNextClip(nextClipName);

	//Check if clipName is different than the current clip m_CurrentClip.name
	if (clipName != m_CurrentClip.name)
	{
		//	Call SetAnimation(clipName)
		SetAnimation(clipName);
	}
}

void ModelAnimator::PlayOnce(const std::wstring& clipName, const std::wstring& nextClipName, std::function<void()> onCompletion)
{
	m_Reversed = false;
	m_IsPlaying = true;
	m_SinglePlay = true;
	m_OnCompletionCallback = onCompletion;
	if (!nextClipName.empty())
	{
		SetNextClip(nextClipName);
		m_HasNext = true;
	}
	else { m_HasNext = false; }


	//Check if clipName is different than the current clip m_CurrentClip.name
	if (clipName != m_CurrentClip.name)
	{
		//	Call SetAnimation(clipName)
		SetAnimation(clipName);
	}
}

void ModelAnimator::ClearNextAnim()
{
	m_NextClip = AnimationClip();
}

void ModelAnimator::SetNextClip(const std::wstring& clipName)
{
	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	for (const auto& animClip : m_AnimationClips)
	{
		if (animClip.name == clipName)
		{
			m_NextClip = animClip;
			return;
		}
	}

	//	Call Reset
	//	Log a warning with an appropriate message
	Reset(false);
	Logger::LogWarning(L"ModelAnimator::SetAnimation(const std::wstring& clipName) > clipName is not found in the m_AnimationClips vector");
}

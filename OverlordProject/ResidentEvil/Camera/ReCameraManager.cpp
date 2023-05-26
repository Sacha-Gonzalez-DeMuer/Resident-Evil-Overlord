#include "stdafx.h"
#include "ReCameraManager.h"
#include "ReCamera.h"

int ReCameraManager::AddVolume(ReCamera* pVolume)
{
	pVolume->SetIdx(static_cast<int>(m_pCameraVolumes.size()));
	m_pCameraVolumes.push_back(pVolume);
	m_pCameras.push_back(pVolume->GetCamera());
	return pVolume->GetIdx();
}

int ReCameraManager::RemoveCamera(CameraComponent* pCamera)
{
	for (auto it = m_pCameras.begin(); it != m_pCameras.end(); ++it)
	{
		if (*it == pCamera)
		{
			m_pCameras.erase(it);
			int idx = static_cast<int>(it - m_pCameras.begin());
			m_pCameraVolumes.erase(m_pCameraVolumes.begin() + idx);
			return idx;
		}
	}
	return -1;
}

void ReCameraManager::SetActiveCamera(UINT index)
{
	if (index < m_pCameraVolumes.size())
	{
		m_pCameras[index]->SetActive(true);
		m_pActiveCamera = m_pCameraVolumes[index];
		OnCameraSwitch.Invoke();
	}
}


void ReCameraManager::SetActiveCamera(ReCamera* pVolume)
{
	pVolume->GetCamera()->SetActive(true);
	m_pActiveCamera = pVolume;
	OnCameraSwitch.Invoke();
}

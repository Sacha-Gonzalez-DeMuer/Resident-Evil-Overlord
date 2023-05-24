#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	m_ParticlesArray = nullptr;

	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	if (!m_pParticleMaterial)
	{
		auto particleMat = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
		m_pParticleMaterial = particleMat;
	}

	CreateVertexBuffer(sceneContext);

	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	// create the vertexbuffer (graphicsdevice->)
	auto hr = sceneContext.d3dContext.pDevice->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);
	HANDLE_ERROR(hr, L"Failed to create vertex buffer");
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	const float particleInterval = (m_EmitterSettings.maxEnergy - m_EmitterSettings.minEnergy) / m_ParticleCount;

	m_LastParticleSpawn += sceneContext.pGameTime->GetElapsed();

	m_ActiveParticles = 0;
	// make subresource contans a pointer to the first vertexparticle element in the buffer
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	mappedResource.pData = m_pVertexBuffer;
	auto hr = sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	HANDLE_ERROR(hr, L"Failed to map vertex buffer");

	//auto* pVertexParticle = static_cast<VertexParticle*>(mappedResource.pData);

	float deltaTime = sceneContext.pGameTime->GetElapsed();
	for (UINT i = 0; i < m_ParticleCount; ++i)
	{

		auto& p = m_ParticlesArray[i];
		if (p.isActive)
		{
			UpdateParticle(p, deltaTime);
		}
		else
		{
			if (m_LastParticleSpawn >= particleInterval)
			{
				SpawnParticle(p);
				m_LastParticleSpawn -= particleInterval;
			}
		}

		if (p.isActive)
		{
			// Calculate the offset of the next available particle in the buffer
			//int nextParticleOffset = m_ActiveParticles * sizeof(VertexParticle);

			// Increment the particle count
			m_ActiveParticles++;

		}
	}

	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	if (!p.isActive) return;

	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy <= 0.0f)
	{
		p.isActive = false;
		return;
	}

	auto velocity = XMLoadFloat3(&m_EmitterSettings.velocity);
	XMVectorAdd(XMLoadFloat3(&p.vertexInfo.Position), XMVectorScale(velocity, elapsedTime));

	float lifePercentage = p.currentEnergy / p.totalEnergy;

	float size = m_EmitterSettings.minSize + (m_EmitterSettings.maxSize - m_EmitterSettings.minSize) * lifePercentage;
	p.vertexInfo.Size = size;

	float alpha = m_EmitterSettings.color.w * lifePercentage;
	p.vertexInfo.Color = XMFLOAT4(m_EmitterSettings.color.x, m_EmitterSettings.color.y, m_EmitterSettings.color.z, alpha);

}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	p.isActive = true;

	p.currentEnergy = p.totalEnergy 
		= MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);


	// position initialization
	XMFLOAT3 randomVec = MathHelper::RandUnitVec3();

	XMVECTOR randomVecV = XMLoadFloat3(&randomVec);
	XMMATRIX randomRot = XMMatrixRotationRollPitchYaw(
		MathHelper::randF(-XM_PI, XM_PI),
		MathHelper::randF(-XM_PI, XM_PI),
		MathHelper::randF(-XM_PI, XM_PI)
	);

	randomVecV = XMVector3TransformNormal(randomVecV, randomRot);
	float distance = MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius);
	XMStoreFloat3(&p.vertexInfo.Position, XMVectorScale(randomVecV, distance));

	// size initialization
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);


	// rotation initialization
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	// color initialization
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	// 1.
	auto VP = sceneContext.pCamera->GetViewProjection();
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", reinterpret_cast<float*>(&VP));

	auto VI = sceneContext.pCamera->GetViewInverse();
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", reinterpret_cast<float*>(&VI));

	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);


	auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	// 2.
	auto& pTechniqueContext = m_pParticleMaterial->GetTechniqueContext();

	// 3.
	pDeviceContext->IASetInputLayout(pTechniqueContext.pInputLayout);

	// 4.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 5.
	UINT stride = sizeof(VertexParticle) * m_ActiveParticles;
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);


	D3DX11_TECHNIQUE_DESC techDesc;
	pTechniqueContext.pTechnique->GetDesc(&techDesc);

	// 6.
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTechniqueContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}
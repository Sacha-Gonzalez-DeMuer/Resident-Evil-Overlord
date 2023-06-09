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

void ParticleEmitterComponent::Emit(const int amount)
{
	int emitted = 0;
	for (UINT i = 0; i < m_ParticleCount; ++i)
	{
		if (emitted >= amount) return;

		auto& p = m_ParticlesArray[i];

		if (!p.isActive)
		{
			SpawnParticle(p);
			++emitted;
		}
	}

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

	auto* pVertexParticles = static_cast<VertexParticle*>(mappedResource.pData);

	float deltaTime = sceneContext.pGameTime->GetElapsed();
	for (UINT i = 0; i < m_ParticleCount; ++i)
	{
		auto& p = m_ParticlesArray[i];
		if (p.isActive)
			UpdateParticle(p, deltaTime);

		if (!p.isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(p);
			m_LastParticleSpawn -= particleInterval;
		}

		if (p.isActive)
			pVertexParticles[m_ActiveParticles++] = p.vertexInfo;
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
	auto newpos = XMVectorAdd(XMLoadFloat3(&p.vertexInfo.Position), XMVectorScale(velocity, elapsedTime));
	XMStoreFloat3(&p.vertexInfo.Position, newpos);

	float lifePercentage = p.currentEnergy / p.totalEnergy;
	p.vertexInfo.Size = p.initialSize * (1.f + (p.sizeChange - 1.f) * (1.f - lifePercentage));
	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercentage;
}

void ParticleEmitterComponent::SpawnParticle(Particle& p, bool forced)
{
	if(!forced && !m_IsPlaying) return;

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

	const auto& rndOffset {XMVectorScale(randomVecV, distance)};
	const auto& worldOffset{ XMVectorAdd(XMLoadFloat3(&GetTransform()->GetWorldPosition()), XMLoadFloat3(&m_EmitterSettings.offset)) };
	XMStoreFloat3(&p.vertexInfo.Position, XMVectorAdd(rndOffset, worldOffset));

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
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture->GetShaderResourceView());


	auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	auto& pTechniqueContext = m_pParticleMaterial->GetTechniqueContext();

	// 3.
	pDeviceContext->IASetInputLayout(pTechniqueContext.pInputLayout);

	// 4.
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 5.
	UINT stride = sizeof(VertexParticle);
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
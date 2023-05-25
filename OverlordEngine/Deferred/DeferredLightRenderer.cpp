#include "stdafx.h"
#include "DeferredLightRenderer.h"

DeferredLightRenderer::~DeferredLightRenderer()
{
	//Release Resources
	SafeRelease(m_pReadOnlyDepthStencilView);
}

void DeferredLightRenderer::Initialize(const D3D11Context& d3dContext)
{
	//Directional LightPass
	m_pDirectionalLightMaterial = MaterialManager::Get()->CreateMaterial<DirectionalLightMaterial>();

	//Volumetric LightPass
	m_pVolumetricLightMaterial = MaterialManager::Get()->CreateMaterial<VolumetricLightMaterial>();
	const auto inputlayoutID = m_pVolumetricLightMaterial->GetTechniqueContext().inputLayoutID;

	//Sphere Light Mesh
	m_pSphereMesh = ContentManager::Load<MeshFilter>(L"Meshes\\UnitSphere.ovm");

	m_pSphereMesh->BuildVertexBuffer(d3dContext, m_pVolumetricLightMaterial);
	m_pSphereVB = m_pSphereMesh->GetVertexBufferData(inputlayoutID).pVertexBuffer;

	m_pSphereMesh->BuildIndexBuffer(d3dContext);
	m_pSphereIB = m_pSphereMesh->GetIndexBuffer(); 

	//Cone Light Mesh
	m_pConeMesh = ContentManager::Load<MeshFilter>(L"Meshes\\UnitCone.ovm");
	m_pConeMesh->BuildVertexBuffer(d3dContext, m_pVolumetricLightMaterial);
	auto& vbData = m_pConeMesh->GetVertexBufferData(inputlayoutID);
	m_pConeVB = vbData.pVertexBuffer;
	m_VertexStride = vbData.VertexStride;
	m_pConeMesh->BuildIndexBuffer(d3dContext);
	m_pConeIB = m_pConeMesh->GetIndexBuffer();
}

void DeferredLightRenderer::DirectionalLightPass(const SceneContext& sceneContext, ID3D11ShaderResourceView* const gbufferSRVs[]) const
{
	//Retrieve Directional light
	const auto& light = sceneContext.pLights->GetDirectionalLight();

	if(light.isEnabled)
	{
		//Prepare Effect
		// 
		//Ambient SRV > Already on Main RenderTarget
		m_pDirectionalLightMaterial->SetVariable_Texture(L"gTextureDiffuse", gbufferSRVs[int(DeferredRenderer::eGBufferId::Diffuse)]);
		m_pDirectionalLightMaterial->SetVariable_Texture(L"gTextureSpecular", gbufferSRVs[int(DeferredRenderer::eGBufferId::Specular)]);
		m_pDirectionalLightMaterial->SetVariable_Texture(L"gTextureNormal", gbufferSRVs[int(DeferredRenderer::eGBufferId::Normal)]);
		m_pDirectionalLightMaterial->SetVariable_Texture(L"gTextureDepth", gbufferSRVs[int(DeferredRenderer::eGBufferId::Depth)]);

		m_pDirectionalLightMaterial->SetVariable_Matrix(L"gMatrixViewProjInv", sceneContext.pCamera->GetViewProjectionInverse());
		m_pDirectionalLightMaterial->SetVariable_Vector(L"gEyePos", sceneContext.pCamera->GetTransform()->GetWorldPosition());
		m_pDirectionalLightMaterial->SetVariable(L"gDirectionalLight", &light, 0, sizeof(Light) - 4);

		//Draw Effect (Full Screen Quad)
		QuadRenderer::Get()->Draw(m_pDirectionalLightMaterial);
	}
}

void DeferredLightRenderer::VolumetricLightPass(const SceneContext& sceneContext, ID3D11ShaderResourceView* const gbufferSRVs[], ID3D11RenderTargetView* pDefaultRTV) const
{
	//Set DefaultRTV WITH Read-Only DSV (no write access to DepthBuffer, but still able to write to StencilBuffer)
	sceneContext.d3dContext.pDeviceContext->OMSetRenderTargets(1, &pDefaultRTV, m_pReadOnlyDepthStencilView);

	//Prepare Effect
	//Ambient SRV > Already on Main RenderTarget
	m_pVolumetricLightMaterial->SetVariable_Texture(L"gTextureDiffuse", gbufferSRVs[int(DeferredRenderer::eGBufferId::Diffuse)]);
	m_pVolumetricLightMaterial->SetVariable_Texture(L"gTextureSpecular", gbufferSRVs[int(DeferredRenderer::eGBufferId::Specular)]);
	m_pVolumetricLightMaterial->SetVariable_Texture(L"gTextureNormal", gbufferSRVs[int(DeferredRenderer::eGBufferId::Normal)]);
	m_pVolumetricLightMaterial->SetVariable_Texture(L"gTextureDepth", gbufferSRVs[int(DeferredRenderer::eGBufferId::Depth)]);

	m_pVolumetricLightMaterial->SetVariable_Texture(L"gShadowMap", ShadowMapRenderer::Get()->GetShadowMap());

	m_pVolumetricLightMaterial->SetVariable_Matrix(L"gMatrixViewProjInv", sceneContext.pCamera->GetViewProjectionInverse());
	m_pVolumetricLightMaterial->SetVariable_Vector(L"gEyePos", sceneContext.pCamera->GetTransform()->GetWorldPosition());

	//Iterate Lights & Render Volumes
	for (auto& light : sceneContext.pLights->GetLights())
	{
		if (!light.isEnabled) continue;

		// clear stencil buffer
		sceneContext.d3dContext.pDeviceContext->ClearDepthStencilView(m_pReadOnlyDepthStencilView, D3D11_CLEAR_STENCIL, 0.f, 1);

		// draw next light
		DrawVolumetricLight(sceneContext, light);
	}
}



void DeferredLightRenderer::DrawVolumetricLight(const SceneContext& sceneContext, const Light& light) const
{
	//Draw Light Volume & Apply Shading
	XMFLOAT3 scale{ light.range, light.range, light.range };
	XMMATRIX rotMatrix{ XMMatrixIdentity() };

	if (light.type == LightType::Spot)
	{
		scale.x = scale.y = tanf(XMConvertToRadians(light.spotLightAngle)) * light.range;
		rotMatrix = MatrixAlignVectors({0.f,0.f, 1.f, 0.f}, light.direction);
	}
	else {
		scale.x;
	}

	const auto transMatrix = XMMatrixTranslation(light.position.x, light.position.y, light.position.z);
	const auto scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	const auto worldMatrix = scaleMatrix * rotMatrix * transMatrix;

	auto wvp = worldMatrix * XMLoadFloat4x4(&sceneContext.pCamera->GetViewProjection());
	m_pVolumetricLightMaterial->SetVariable_Matrix(L"gWorldViewProjection", reinterpret_cast<const float*>(&wvp));
	m_pVolumetricLightMaterial->SetVariable(L"gCurrentLight", &light, 0, sizeof(Light) - 4);

		
	const auto pDeviceContext = sceneContext.d3dContext.pDeviceContext;
	auto& techContext = m_pVolumetricLightMaterial->GetTechniqueContext();

	//set inputlayout
	pDeviceContext->IASetInputLayout(techContext.pInputLayout);

	//set vertexbuffer 
	const UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0,1,light.type == LightType::Point ? &m_pSphereVB : &m_pConeVB, &m_VertexStride, &offset);

	// set indexbuffer
	pDeviceContext->IASetIndexBuffer(light.type == LightType::Point ? m_pSphereIB : m_pConeIB, DXGI_FORMAT_R32_UINT, 0);

	// set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	techContext.pTechnique->GetDesc(&techDesc);
	for (UINT p{ 0 }; p < techDesc.Passes; ++p)
	{
		techContext.pTechnique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(light.type == LightType::Point ? m_pSphereMesh->GetIndexCount() : m_pConeMesh->GetIndexCount(), 0, 0);
	}
}

void DeferredLightRenderer::CreateReadOnlyDSV(const D3D11Context& d3dContext, ID3D11Resource* pDepthResource, DXGI_FORMAT format)
{
	//Create DSV with Read-Only Depth (m_pReadOnlyDepthStencilView)
	SafeRelease(m_pReadOnlyDepthStencilView);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV{};
	descDSV.Format = format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = D3D11_DSV_READ_ONLY_DEPTH; //depth read only, stencil read/write
	descDSV.Texture2D.MipSlice = 0;

	HANDLE_ERROR(d3dContext.pDevice->CreateDepthStencilView(pDepthResource, &descDSV, &m_pReadOnlyDepthStencilView));
}

void VolumetricLightMaterial::OnUpdateModelVariables(const SceneContext&, const ModelComponent*) const
{
}
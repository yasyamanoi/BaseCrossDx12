/*!
@file ShadowmapComp.cpp
@brief シャドウマップコンポーネント　実体
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"
//#include "Project.h"

namespace basecross {


	float Shadowmap::m_lightHeight(200.0f);
	float Shadowmap::m_lightNear(1.0f);
	float Shadowmap::m_lightFar(220.0f);
	float Shadowmap::m_viewWidth(16.0f);
	float Shadowmap::m_viewHeight(16.0f);
	float Shadowmap::m_posAdjustment(0.1f);


	//--------------------------------------------------------------------------------------
	///	Shadowmap
	//--------------------------------------------------------------------------------------
	IMPLEMENT_DX12SHADER(PNTShadowmap, App::GetShadersDir() + L"VSShadowmap.cso")

	Shadowmap::Shadowmap(const std::shared_ptr<GameObject>& gameObjectPtr) :
		Component(gameObjectPtr)
	{
	}

	void Shadowmap::OnCreate() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto pBaseScene = BaseScene::Get();
		auto& frameResources = pBaseScene->GetFrameResources();
		auto pBaseDevice = BaseDevice::GetBaseDevice();
		//シャドウマップのコンスタントバッファ
		for (size_t i = 0; i < BaseDevice::FrameCount; i++) {
			m_shadowConstantBufferIndex =
				frameResources[i]->AddBaseConstantBufferSet<ShadowConstantBuffer>(pBaseDevice->GetD3D12Device());
		}
	}

	void Shadowmap::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto ptrStage = scene->GetActiveStage();
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (ptrStage) {
			myCamera = std::dynamic_pointer_cast<PerspecCamera>(ptrStage->GetTargetCamera());
			if (!myCamera) {
				return;
			}
			myLightSet = ptrStage->GetTargetLightSet();
		}
		else {
			return;
		}

		auto gameObject = m_gameObject.lock();
		if (gameObject) {
			//Transformコンポーネントを取り出す
			auto ptrTrans = gameObject->GetComponent<Transform>();
			auto& param = ptrTrans->GetTransParam();

			//シャドウマップのコンスタントバッファ
			{

				m_shadowConstantBuffer = {};

				auto lights = myLightSet;
				auto light = lights->GetMainBaseLight();

				//位置の取得
				bsm::Vec3 pos = param.position;
				bsm::Vec3 posSpan = (bsm::Vec3)light.m_directional;
				posSpan *= bsm::Vec3(Shadowmap::m_posAdjustment);
				pos += posSpan;
				//ワールド行列の決定
				auto param_tmp = param;
				param_tmp.position = pos;

				auto world = XMMatrixAffineTransformation(
					param_tmp.scale,
					param_tmp.rotOrigin,
					param_tmp.quaternion,
					param_tmp.position
				);

				bsm::Vec3 lightDir = bsm::Vec3(light.m_directional) * bsm::Vec3(-1.0f);

				auto camera = myCamera;
				bsm::Vec3 lightAt(camera->GetAt());
				bsm::Vec3 lightDirTmp(lightDir * bsm::Vec3(Shadowmap::m_lightHeight));
				bsm::Vec3 lightEye(lightAt + lightDirTmp);
				auto width = viewport.Width;
				auto height = viewport.Height;

				auto lightView
					= XMMatrixLookAtLH(lightEye, lightAt, bsm::Vec3(0, 1.0f, 0));

				auto lightProj =
					XMMatrixOrthographicLH(
						Shadowmap::m_viewWidth,
						Shadowmap::m_viewHeight,
						Shadowmap::m_lightNear,
						Shadowmap::m_lightFar);
				m_shadowConstantBuffer.world = bsm::Mat4x4(XMMatrixTranspose(world));
				m_shadowConstantBuffer.view = bsm::Mat4x4(XMMatrixTranspose(lightView));
				m_shadowConstantBuffer.projection = bsm::Mat4x4(XMMatrixTranspose(lightProj));
			}
		}
	}

	void Shadowmap::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シャドウマップ
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_pBaseConstantBufferWO,
			&m_shadowConstantBuffer, sizeof(m_shadowConstantBuffer));
	}

	void Shadowmap::OnShadowDraw() {
		ID3D12GraphicsCommandList* pCommandList = BaseScene::Get()->m_pTgtCommandList;
		auto mesh = m_mesh.lock();
		if (mesh) {
			auto pBaseScene = BaseScene::Get();
			auto pCurrentFrameResource = pBaseScene->GetCurrentFrameResource();
			//Cbv
			// Set shadow constant buffer.
			pCommandList->SetGraphicsRootConstantBufferView(pBaseScene->GetGpuSlotID(L"b0"),
				pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_baseConstantBuffer->GetGPUVirtualAddress());
			// Draw
			pCommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
			pCommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
			pCommandList->DrawIndexedInstanced(mesh->GetNumIndices(), 1, 0, 0, 0);
		}
	}

}
// end namespace basecross

/*!
@file ShadowmapComp.cpp
@brief シャドウマップコンポーネント　実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	Shadowmap
	//--------------------------------------------------------------------------------------
	IMPLEMENT_DX12SHADER(PNTShadowmap, App::GetShadersDir() + L"VSShadowmap.cso")

	ShadowmapComp::ShadowmapComp(const std::shared_ptr<GameObject>& gameObjectPtr):
		Component(gameObjectPtr)
	{ }

	void ShadowmapComp::OnCreate() {
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

	void ShadowmapComp::OnUpdateConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto gameStage = std::dynamic_pointer_cast<GameStage>(scene->GetActiveStage());
		auto& viewport = scene->GetViewport();
		std::shared_ptr<PerspecCamera> myCamera;
		std::shared_ptr<LightSet> myLightSet;
		if (gameStage) {
			myCamera = gameStage->GetMyCamera();
			myLightSet = gameStage->GetLightSet();

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
				Vec3 pos = param.position;
				Vec3 posSpan = (Vec3)light.m_directional;
				posSpan *= Vec3(Shadowmap::GetPosAdjustment());
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

				Vec3 lightDir = Vec3(light.m_directional) * Vec3(-1.0f);

				auto camera = myCamera;
				Vec3 lightAt(camera->GetAt());
				Vec3 lightDirTmp(lightDir * Vec3(Shadowmap::GetLightHeight()));
				Vec3 lightEye(lightAt + lightDirTmp);
				auto width = viewport.Width;
				auto height = viewport.Height;

				auto lightView
					= XMMatrixLookAtLH(lightEye, lightAt, Vec3(0, 1.0f, 0));

				auto lightProj =
					XMMatrixOrthographicLH(
						Shadowmap::GetViewWidth(),
						Shadowmap::GetViewHeight(),
						Shadowmap::GetLightNear(),
						Shadowmap::GetLightFar());
				m_shadowConstantBuffer.world = Mat4x4(XMMatrixTranspose(world));
				m_shadowConstantBuffer.view = Mat4x4(XMMatrixTranspose(lightView));
				m_shadowConstantBuffer.projection = Mat4x4(XMMatrixTranspose(lightProj));
			}
		}
	}

	void ShadowmapComp::OnCommitConstantBuffers() {
		auto scene = dynamic_cast<Scene*>(BaseScene::Get());
		auto pCurrentFrameResource = scene->GetCurrentFrameResource();
		//シャドウマップ
		memcpy(pCurrentFrameResource->m_baseConstantBufferSetVec[m_shadowConstantBufferIndex].m_pBaseConstantBufferWO,
			&m_shadowConstantBuffer, sizeof(m_shadowConstantBuffer));
	}

	void ShadowmapComp::OnShadowDraw() {
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

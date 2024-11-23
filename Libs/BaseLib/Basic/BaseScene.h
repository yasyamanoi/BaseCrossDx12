/*!
@file BaseScene.h
@brief シーン親クラス
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
*/

#pragma once

#include "stdafx.h"


namespace basecross {

	class BaseFrame;
	class PrimDevice;

	static const UINT NumNullSrvs = 2; // Null デスクプリタ
	static const UINT NumLights = 1; //ライト数

	//コマンドリスト数
	static const int CommandListCount = 3;
	static const int CommandListPre = 0;
	static const int CommandListMid = 1;
	static const int CommandListPost = 2;

	namespace SceneEnums
	{
		namespace RenderPass {
			enum Value { Scene = 0, Shadow, Postprocess, Count };
		}

		namespace DepthGenPass {
			enum Value { Scene = 0, Shadow, Count };
		}

		namespace RootSignature {
			enum { ShadowPass = 0, ScenePass, SpritePass, PostprocessPass, Count };
		};

		namespace VertexBuffer {
			enum Value { ScreenQuad = 0, Count };
		}
	}
	using namespace SceneEnums;

	//--------------------------------------------------------------------------------------
	///	BaseConstantコンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct BaseConstant
	{
		bsm::Mat4x4 world; // ワールド行列
		bsm::Mat4x4 view; // ビュー行列
		bsm::Mat4x4 projection; // 射影行列
		bsm::Col4 emissive; // エミッシブ色
		bsm::Col4 diffuse; // デフューズ色
		bsm::Col4 specular;// スペキュラー
		XMUINT4 activeFlg; // テクスチャ=xがアクティブかどうか
		bsm::Vec4 lightDir;// ライイト方向
		bsm::Vec4 lightPos;// ライト位置
		bsm::Vec4 eyePos;// eyeの位置
		bsm::Mat4x4 lightView;// ライトビュー行列
		bsm::Mat4x4 lightProjection;// ライト射影行列
		bsm::Vec4 Bones[3 * 72];// Bone配列
		BaseConstant() {
			memset(this, 0, sizeof(BaseConstant));
			diffuse = bsm::Col4(1.0f, 1.0f, 1.0f, 1.0f);
		};
	};

	//--------------------------------------------------------------------------------------
	///	2D用コンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct Base2DConstant {
		bsm::Mat4x4 transform; // 変換行列（row_majorで設定）
		bsm::Col4 emissive; // エミッシブ色
		bsm::Col4 diffuse; // デフューズ色

	};


	//--------------------------------------------------------------------------------------
	///	ポストプロセス用コンスタントバッファ構造体
	//--------------------------------------------------------------------------------------
	struct PostprocessConstantBuffer
	{
		XMFLOAT4 lightPosition;
		XMFLOAT4 cameraPosition;
		XMFLOAT4X4 viewInverse;
		XMFLOAT4X4 projInverse;
		XMFLOAT4X4 viewProjInverseAtNearZ1;
		float fogDensity;
	};


	//--------------------------------------------------------------------------------------
	///	ベースシーン（親クラス）
	//--------------------------------------------------------------------------------------
	class BaseScene
	{
	protected:
		BaseScene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~BaseScene();
	public:
		float m_fogDensity; //フォグ密度
		//初期化
		virtual void Initialize(ID3D12Device* pDevice, ID3D12CommandQueue* pDirectCommandQueue, ID3D12GraphicsCommandList* pCommandList, UINT frameIndex);
		//windowに依存するリソースの登録
		virtual void LoadSizeDependentResources(ID3D12Device* pDevice, ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height);
		//windowに依存するリソースの解放
		virtual void ReleaseSizeDependentResources();
		//フレームインデックスの設定
		void SetFrameIndex(UINT frameIndex);
		//D3Dオブジェクトのリリース
		void ReleaseD3DObjects();
		//キーボードを押す
		void KeyDown(UINT8 key);
		//キーボードを離す
		void KeyUp(UINT8 key);
		//更新（純粋仮想関数）
		virtual void Update(double elapsedTime) = 0;
		//描画（仮想関数にしない）
		void Render(ID3D12CommandQueue* pCommandQueue, bool setBackbufferReadyForPresent);
		//自分自身の取得
		static BaseScene* Get() { return pBaseScene; }
		//Rtvディスクリプタヒープの取得
		ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() const {
			return m_rtvHeap;
		}
		//Dsvディスクリプタヒープの取得
		ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const {
			return m_dsvHeap;
		}
		//CbvSrvUavディスクリプタヒープの取得
		ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const {
			return m_cbvSrvUavHeap;
		}
		//サンプラーディスクリプタヒープの取得
		ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap() const {
			return m_samplerHeap;
		}
		//Rtvディスクリプタヒープのインクリメントサイズの取得
		UINT GetRtvDescriptorHandleIncrementSize() const {
			return m_rtvDescriptorSize;
		}
		//Dsvディスクリプタヒープのインクリメントサイズの取得
		UINT GetDsvDescriptorHandleIncrementSize() const {
			return m_dsvDescriptorSize;
		}
		//CbvSrvUavディスクリプタヒープのインクリメントサイズの取得
		UINT GetCbvSrvUavDescriptorHandleIncrementSize() const {
			return m_cbvSrvDescriptorSize;
		}
		//サンプラーディスクリプタヒープのインクリメントサイズの取得
		UINT GetSamplerDescriptorHandleIncrementSize() const {
			return m_samplerDescriptorSize;
		}
		//Srv(シェーダーリソースビュー)の次のインデックスの取得
		UINT GetSrvNextIndex();
		//Cbv(コンスタントバッファビュー)の次のインデックスの取得
		UINT GetCbvUavNextIndex();
		//サンプラーの次のインデックスの取得
		UINT GetSamplerNextIndex();
		//指定したサンプラーのインデックスの取得
		UINT GetSamplerIndex(const std::wstring& key);
		//GPUのスロットIDの設定
		void SetGpuSlot(const std::wstring& key, UINT val) {
			m_gpuSlotMap[key] = val;
		}
		//GPUのスロットIDの取得
		UINT GetGpuSlotID(const std::wstring& key) {
			auto it = m_gpuSlotMap.begin();
			while (it != m_gpuSlotMap.end()) {
				if (it->first == key) {
					return it->second;
				}
				it++;
			}
			throw BaseException(
				L"そのキーはGPUスロットにありません",
				key,
				L"BaseDevice::GetGpuSlotID()"
			);
			return 0;
		}
		UINT GetFrameCount() const 
		{
			return m_frameCount;
		}
		UINT GetFrameIndex() const {
			return m_frameIndex;
		}
		std::vector<std::unique_ptr<BaseFrame>>& GetframeResources() {
			return m_frameResources;
		}
		ComPtr<ID3D12RootSignature>* GetRootSignatures(){
			return m_rootSignatures;
		}
		const CD3DX12_VIEWPORT& GetViewport() const {
			return m_viewport;
		}
		const CD3DX12_RECT& GetScissorRect() const {
			return m_scissorRect;
		}
		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferRtvCpuHandle()
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
		}
		virtual UINT GetNumRtvDescriptors() const
		{
			return m_frameCount;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE* GetDepthDsvs() {
			return m_depthDsvs;
		}
		//UI文字列の更新
		virtual void UpdateUI(std::wstring& uiText){}
	protected:
		const UINT m_frameCount; //フレーム数
		//ポストプロセス用の頂点定義
		struct Vertex
		{
			XMFLOAT4 position;
		};
		//コマンドリストの配列
		ID3D12CommandList* m_batchSubmit[2 + CommandListCount];   // 2: shadowCommandLists, sceneCommandLists

		CD3DX12_VIEWPORT m_viewport;//ビューポート
		CD3DX12_RECT m_scissorRect;//シザー矩形

		// D3D オブジェクト
		ComPtr<ID3D12GraphicsCommandList> m_commandLists[CommandListCount];
		ComPtr<ID3D12GraphicsCommandList> m_shadowCommandList;
		ComPtr<ID3D12GraphicsCommandList> m_sceneCommandList;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
		ComPtr<ID3D12Resource> m_depthTextures[SceneEnums::DepthGenPass::Count];
		ComPtr<ID3D12RootSignature> m_rootSignatures[SceneEnums::RootSignature::Count];
//		ComPtr<ID3D12PipelineState> m_pipelineStates[SceneEnums::RenderPass::Count];
		ComPtr<ID3D12Resource> m_vertexBuffers[SceneEnums::VertexBuffer::Count];
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferViews[SceneEnums::VertexBuffer::Count];

		// Heap オブジェクト
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvSrvDescriptorSize;
		UINT m_samplerDescriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE m_depthDsvs[SceneEnums::DepthGenPass::Count];
		D3D12_CPU_DESCRIPTOR_HANDLE m_depthSrvCpuHandles[SceneEnums::DepthGenPass::Count];
		D3D12_GPU_DESCRIPTOR_HANDLE m_depthSrvGpuHandles[SceneEnums::DepthGenPass::Count];

		// フレームリソース
		std::vector<std::unique_ptr<BaseFrame>> m_frameResources;
		BaseFrame* m_pCurrentBaseFrame;
		UINT m_frameIndex;

		// App resources.
		static const float s_clearColor[4];

		// Window state
		bool m_windowVisible;
		bool m_windowedMode;

		static BaseScene* pBaseScene;

		//以下、追加
		//cbvSrvUav管理用
		//CbvSrvUavの最大値
		const UINT m_cbvSrvUavMax = 4096;
		//Srvの開始インデックス(NullSrv用とdepthSrv用を前に置く)
		//NullSrv==2
		//_countof(m_depthSrvCpuHandles)==2
		const UINT m_srvStartIndex = 4;
		//Srvの最大値
		const UINT m_srvMax = 1024;
		//Srvの発行インデックス
		UINT m_srvSendIndex;
		//CbvUavの開始インデックス
		const UINT m_cbvUavStartIndex = 1024;
		//CbvUavの最大値
		const UINT m_cbvUavMax = 4096;
		//CbvUavの発行インデックス
		UINT m_cbvUavSendIndex;
		//sampler管理用
		std::map<std::wstring, UINT> m_samplerMap;
		const UINT m_samplerMax = 128;
		UINT m_samplerSendIndex;

		//ルートシグネチャで設定されるGPUスロットのマップ
		std::map<std::wstring, UINT> m_gpuSlotMap;

		inline HRESULT CreateDepthStencilTexture2D(
			ID3D12Device* pDevice,
			UINT width,
			UINT height,
			DXGI_FORMAT typelessFormat,
			DXGI_FORMAT dsvFormat,
			DXGI_FORMAT srvFormat,
			ID3D12Resource** ppResource,
			D3D12_CPU_DESCRIPTOR_HANDLE cpuDsvHandle,
			D3D12_CPU_DESCRIPTOR_HANDLE cpuSrvHandle,
			D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_DEPTH_WRITE,
			float initDepthValue = 1.0f,
			UINT8 initStencilValue = 0)
		{
			try
			{
				*ppResource = nullptr;

				CD3DX12_RESOURCE_DESC texDesc(
					D3D12_RESOURCE_DIMENSION_TEXTURE2D,
					0,
					width,
					height,
					1,
					1,
					typelessFormat,
					1,
					0,
					D3D12_TEXTURE_LAYOUT_UNKNOWN,
					D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

				ThrowIfFailed(pDevice->CreateCommittedResource(
					&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
					D3D12_HEAP_FLAG_NONE,
					&texDesc,
					initState,
					&CD3DX12_CLEAR_VALUE(dsvFormat, initDepthValue, initStencilValue), // Performance tip: Tell the runtime at resource creation the desired clear value.
					IID_PPV_ARGS(ppResource)));

				// Create a depth stencil view (DSV).
				D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = dsvFormat;
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = 0;
				pDevice->CreateDepthStencilView(*ppResource, &dsvDesc, cpuDsvHandle);

				// Create a shader resource view (SRV).
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = srvFormat;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				pDevice->CreateShaderResourceView(*ppResource, &srvDesc, cpuSrvHandle);
			}
			catch (HrException& e)
			{
				SAFE_RELEASE(*ppResource);
				return e.Error();
			}
			return S_OK;
		}


		//これよ仮想関数
		//以下、D3Dリソースの作成
		virtual void CreateDescriptorHeaps(ID3D12Device* pDevice);
		virtual void CreateCommandLists(ID3D12Device* pDevice);
		virtual void CreateRootSignatures(ID3D12Device* pDevice);
		virtual void CreatePipelineStates(ID3D12Device* pDevice) = 0;
		virtual void CreateBaseFrames(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
		virtual void CreateSceneResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) = 0;
		virtual void CreatePostprocessPassResources(ID3D12Device* pDevice);
		virtual void CreateSamplers(ID3D12Device* pDevice);

		//フレーム処理の準備
		virtual void BeginFrame();
		//フレームのリセット
		virtual void MidFrame();
		//フレームの終了
		virtual void EndFrame(bool setBackbufferReadyForPresent);

		//シャドウとシーンの描画処理
		virtual void WorkerThread();
		//描画処理（シャドウ）
		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList) = 0;
		//描画処理（シーン）
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList) = 0;
		//描画処理（ポストプロセス）
		virtual void PostprocessPass(ID3D12GraphicsCommandList* pCommandList);



	};
}
using namespace basecross;
// end namespace basecross
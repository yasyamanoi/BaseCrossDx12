//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

/*!
@file BaseScene.h
@brief �V�[���e�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	using namespace DirectX;
	class FrameResource;
	class PrimDevice;
	class Stage;

	//--------------------------------------------------------------------------------------
	///	�V�[���N���X�i�e�j
	//--------------------------------------------------------------------------------------
	class BaseScene
	{
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	frameCount	�t���[����
		@param[in]	pPrimDevice	��{�f�o�C�X
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		BaseScene(UINT frameCount, PrimDevice* pPrimDevice);
		virtual ~BaseScene();
		ID3D12GraphicsCommandList* m_pTgtCommandList;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	������
		@param[in]	pDevice	ID3D12Device�̃|�C���^
		@param[in]	pDirectCommandQueue	�R�}���h�L���[
		@param[in]	pCommandList	�R�}���h���X�g
		@param[in]	frameIndex	�t���[���ԍ�
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void Initialize(ID3D12Device* pDevice, ID3D12CommandQueue* pDirectCommandQueue, ID3D12GraphicsCommandList* pCommandList, UINT frameIndex);
		virtual void LoadSizeDependentResources(ID3D12Device* pDevice, ComPtr<ID3D12Resource>* ppRenderTargets, UINT width, UINT height);
		virtual void ReleaseSizeDependentResources();
		void SetFrameIndex(UINT frameIndex);
		void ReleaseD3DObjects();
		void KeyDown(UINT8 key);
		void KeyUp(UINT8 key);

		std::shared_ptr<Stage> GetActiveStage(bool ExceptionActive) const {
			if (!m_activeStage) {
				//�A�N�e�B�u�ȃX�e�[�W�������Ȃ�
				if (ExceptionActive) {
					throw BaseException(
						L"�A�N�e�B�u�ȃX�e�[�W������܂���",
						L"if(!m_activeStage)",
						L"BaseScene::GetActiveStage()"
					);
				}
				else {
					return nullptr;
				}
			}
			return m_activeStage;
		}

		virtual void UpdateUI(std::unique_ptr<UILayer>& uiLayer) = 0;
		virtual void Update(double elapsedTime) = 0;
		virtual void Destroy() {}
		virtual void Render(ID3D12CommandQueue* pCommandQueue, bool setBackbufferReadyForPresent);
		virtual void SetToBefore() {}
		static BaseScene* Get(){ return s_baseScene; }
		std::vector<std::unique_ptr<FrameResource>>& GetFrameResources() {
			return m_frameResources;
		}
		FrameResource* GetCurrentFrameResource()const {
			return m_pCurrentFrameResource;
		}
		//Rtv�f�B�X�N���v�^�q�[�v�̎擾
		ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() const {
			return m_rtvHeap;
		}
		//Dsv�f�B�X�N���v�^�q�[�v�̎擾
		ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const {
			return m_dsvHeap;
		}
		//CbvSrvUav�f�B�X�N���v�^�q�[�v�̎擾
		ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const {
			return m_cbvSrvHeap;
		}
		//�T���v���[�f�B�X�N���v�^�q�[�v�̎擾
		ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap() const {
			return m_samplerHeap;
		}
		//Rtv�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		UINT GetRtvDescriptorHandleIncrementSize() const {
			return m_rtvDescriptorSize;
		}
		//Dsv�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		UINT GetDsvDescriptorHandleIncrementSize() const {
			return m_dsvDescriptorSize;
		}
		//CbvSrvUav�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		UINT GetCbvSrvUavDescriptorHandleIncrementSize() const {
			return m_cbvSrvDescriptorSize;
		}
		//�T���v���[�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		UINT GetSamplerDescriptorHandleIncrementSize() const {
			return m_samplerDescriptorSize;
		}
		inline CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferRtvCpuHandle()
		{
			return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
		}

		//Srv(�V�F�[�_�[���\�[�X�r���[)�̎��̃C���f�b�N�X�̎擾
		UINT GetSrvNextIndex();
		//Cbv(�R���X�^���g�o�b�t�@�r���[)�̎��̃C���f�b�N�X�̎擾
		UINT GetCbvUavNextIndex();
		//�T���v���[�̎��̃C���f�b�N�X�̎擾
		UINT GetSamplerNextIndex();
		//�w�肵���T���v���[�̃C���f�b�N�X�̎擾
		UINT GetSamplerIndex(const std::wstring& key);
		//�r���[�|�[�g�̎擾
		const CD3DX12_VIEWPORT& GetViewport()const  {
			return m_viewport;
		}
		//�V�U�[���N�g�̎擾
		const CD3DX12_RECT& GetScissorRect() {
			return m_scissorRect;
		}
		//CPU���f�v�X�n���h���̎擾
		D3D12_CPU_DESCRIPTOR_HANDLE* GetDepthDsvs() {
			return m_depthDsvs;
		}
		//CPU���V�F�[�_�[���\�[�X�f�v�X�n���h���̎擾
		D3D12_CPU_DESCRIPTOR_HANDLE* GetDepthSrvCpuHandles() {
			return m_depthSrvCpuHandles;
		}
		//GPU���V�F�[�_�[���\�[�X�f�v�X�n���h���̎擾
		D3D12_GPU_DESCRIPTOR_HANDLE* GetDepthSrvGpuHandles() {
			return m_depthSrvGpuHandles;
		}
		//GPU�̃X���b�gID�̐ݒ�
		void SetGpuSlot(const std::wstring& key, UINT val) {
			m_gpuSlotMap[key] = val;
		}
		//GPU�̃X���b�gID�̎擾
		UINT GetGpuSlotID(const std::wstring& key) {
			auto it = m_gpuSlotMap.begin();
			while (it != m_gpuSlotMap.end()) {
				if (it->first == key) {
					return it->second;
				}
				it++;
			}
			throw BaseException(
				L"���̃L�[��GPU�X���b�g�ɂ���܂���",
				key,
				L"BaseDevice::GetGpuSlotID()"
			);
			return 0;
		}
		template<typename T, typename... Ts>
		std::shared_ptr<T> ResetActiveStage(Ts&&... params) {
			auto actStagePtr = GetActiveStage(false);
			if (actStagePtr) {
				//�j����`����
				actStagePtr->OnDestroy();
				actStagePtr = nullptr;
			}
			auto ptr = ObjectFactory::Create<T>(params...);
			auto stagePtr = std::dynamic_pointer_cast<Stage>(ptr);
			if (!stagePtr) {
				throw BaseException(
					L"�ȉ���Stage�Ɍ^�L���X�g�ł��܂���B",
					Util::GetWSTypeName<T>(),
					L"Scene::ResetActiveStage<T>()"
				);
			}
			SetActiveStage(stagePtr);
			return ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���b�V�������\�[�X�o�^����
		@param[in]	key	���\�[�X�̃L�[
		@param[in]	mesh	���b�V��
		@param[in]	keyCheck	�L�[�̏d���`�F�b�N���邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RegisterMesh(const std::wstring& key, const std::shared_ptr<BaseMesh>& mesh, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�o�^����Ă��郁�b�V�����擾����
		@param[in]	key	���\�[�X�̃L�[
		@return	���b�V��
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<BaseMesh> GetMesh(const std::wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�N�X�`�������\�[�X�o�^����
		@param[in]	key	���\�[�X�̃L�[
		@param[in]	texture	�e�N�X�`��
		@param[in]	keyCheck	�L�[�̏d���`�F�b�N���邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RegisterTexture(const std::wstring& key, const std::shared_ptr<BaseTexture>& texture, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�o�^����Ă���e�N�X�`�����擾����
		@param[in]	key	���\�[�X�̃L�[
		@return	�e�N�X�`��
		*/
		//--------------------------------------------------------------------------------------
		std::shared_ptr<BaseTexture> GetTexture(const std::wstring& key);

		//--------------------------------------------------------------------------------------
		/*!
		@brief	PhysX�I�u�W�F�N�g�𓾂�
		@return	PhysX�I�u�W�F�N�g�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		physx::PxPhysics* GetPxPhysics() {
			return m_pPhysics;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	PhysX�V�[���𓾂�
		@return	PhysX�V�[���̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		physx::PxScene* GetPxScene() {
			return m_pScene;
		}
		static double GetElapsedTime() {
			return s_elapsedTime;
		}
		float m_fogDensity;
	protected:
		UINT m_frameCount;

		// *2: shadowCommandLists, sceneCommandLists
		ID3D12CommandList* m_batchSubmit[2 + CommandListCount];   
		// Pipeline objects.
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		// D3D objects.
		ComPtr<ID3D12GraphicsCommandList> m_commandLists[CommandListCount];
		ComPtr<ID3D12GraphicsCommandList> m_shadowCommandList;
		ComPtr<ID3D12GraphicsCommandList> m_sceneCommandList;
		std::vector<ComPtr<ID3D12Resource>> m_renderTargets;
		ComPtr<ID3D12Resource> m_depthTextures[SceneEnums::DepthGenPass::Count];

		// Heap objects.
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;

		UINT m_rtvDescriptorSize;
		UINT m_dsvDescriptorSize;
		UINT m_cbvSrvDescriptorSize;
		UINT m_samplerDescriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE m_depthDsvs[SceneEnums::DepthGenPass::Count];
		D3D12_CPU_DESCRIPTOR_HANDLE m_depthSrvCpuHandles[SceneEnums::DepthGenPass::Count];
		D3D12_GPU_DESCRIPTOR_HANDLE m_depthSrvGpuHandles[SceneEnums::DepthGenPass::Count];

		// Frame resources.
		std::vector<std::unique_ptr<FrameResource>> m_frameResources;
		FrameResource* m_pCurrentFrameResource;
		UINT m_frameIndex;

		// App resources.
		PrimDevice* m_pPrimDevice;
		//Stage
		std::shared_ptr<Stage> m_activeStage;
		//mesh map
		std::map<const std::wstring, std::shared_ptr<BaseMesh> > m_meshMap;
		//texture map
		std::map<const std::wstring, std::shared_ptr<BaseTexture> > m_textureMap;


		static const float s_clearColor[4];

		// Window state
		bool m_windowVisible;
		bool m_windowedMode;

		//cbvSrvUav�Ǘ��p
		//CbvSrvUav�̍ő�l
		const UINT m_cbvSrvUavMax = 4096;
		//Srv�̊J�n�C���f�b�N�X(NullSrv�p��depthSrv�p��O�ɒu��)
		//NullSrv==2
		//_countof(m_depthSrvCpuHandles)==2
		// NumNullSrvs + _countof(m_depthSrvCpuHandles)
		const UINT m_srvStartIndex = NumNullSrvs + _countof(m_depthSrvCpuHandles);
		//Srv�̍ő�l
		const UINT m_srvMax = 1024;
		//Srv�̔��s�C���f�b�N�X
		UINT m_srvSendIndex;
		//CbvUav�̊J�n�C���f�b�N�X
		const UINT m_cbvUavStartIndex = 1024;
		//CbvUav�̍ő�l
		const UINT m_cbvUavMax = 2048;
		//CbvUav�̔��s�C���f�b�N�X
		UINT m_cbvUavSendIndex;
		//sampler�Ǘ��p
		std::map<std::wstring, UINT> m_samplerMap;
		const UINT m_samplerMax = 128;
		UINT m_samplerSendIndex;

		//���[�g�V�O�l�`���Őݒ肳���GPU�X���b�g�̃}�b�v
		std::map<std::wstring, UINT> m_gpuSlotMap;

		HANDLE m_workerBeginRenderFrame;
		HANDLE m_workerFinishShadowPass;
		HANDLE m_workerFinishedScenePass;
		HANDLE m_threadHandles;
		// Singleton object so that worker threads can share members.
		static BaseScene* s_baseScene;
		//elapsedTime
		static double s_elapsedTime;

		//�ȉ��APhysX�֘A
		// PhysX���ŗ��p����A���P�[�^�[
		physx::PxDefaultAllocator m_defaultAllocator;
		// �G���[���p�̃R�[���o�b�N�ŃG���[���e�������Ă�
		physx::PxDefaultErrorCallback m_defaultErrorCallback;
		// ��ʃ��x����SDK(PxPhysics�Ȃ�)���C���X�^���X������ۂɕK�v
		physx::PxFoundation* m_pFoundation = nullptr;
		// ���ۂɕ������Z���s��
		physx::PxPhysics* m_pPhysics = nullptr;
		// �V�~�����[�V�������ǂ��������邩�̐ݒ�Ń}���`�X���b�h�̐ݒ���ł���
		physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
		// �V�~�����[�V���������Ԃ̒P�ʂ�Actor�̒ǉ��Ȃǂ������ōs��
		physx::PxScene* m_pScene = nullptr;


		// Updates the shadow copies of the constant buffers.
		virtual void UpdateConstantBuffers() = 0; 
		// Commits the shadows copies of the constant buffers to GPU-visible memory for the current frame.
		virtual void CommitConstantBuffers() = 0; 
		virtual void DrawInScattering(ID3D12GraphicsCommandList* pCommandList, const D3D12_CPU_DESCRIPTOR_HANDLE& renderTargetHandle);
		void WorkerThread();

		virtual void ShadowPass(ID3D12GraphicsCommandList* pCommandList) = 0;
		virtual void ScenePass(ID3D12GraphicsCommandList* pCommandList) = 0;
		virtual void PostprocessPass(ID3D12GraphicsCommandList* pCommandList);



		virtual void BeginFrame();
		virtual void MidFrame();
		virtual void EndFrame(bool setBackbufferReadyForPresent);

		virtual void CreateDescriptorHeaps(ID3D12Device* pDevice);
		virtual void CreateCommandLists(ID3D12Device* pDevice);
		virtual void CreateRootSignatures(ID3D12Device* pDevice);
		virtual void CreateFrameResources(ID3D12Device* pDevice, ID3D12CommandQueue* pCommandQueue);
		virtual void CreateBasicResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
		virtual void CreateDefaultResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
		virtual void CreateAssetResources(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList) = 0;
		virtual void CreatePostprocessPassResources(ID3D12Device* pDevice);
		virtual void CreateSamplers(ID3D12Device* pDevice);

		void SetActiveStage(const std::shared_ptr<Stage>& stage) {
			m_activeStage = stage;
		}

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

		virtual UINT GetNumRtvDescriptors() const
		{
			return m_frameCount;
		}

		virtual UINT GetNumCbvSrvUavDescriptors() const
		{
			return m_cbvSrvUavMax;
		}
		virtual UINT GetNumSamplerDescriptors() const
		{
			return m_samplerMax;
		}
	};
}
// end namespace basecross

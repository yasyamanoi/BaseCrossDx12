/*!
@file BaseDevice.h
@brief �f�o�C�X�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	class BaseMesh;
	class BaseTexture;
	struct BaseFrame;

	//--------------------------------------------------------------------------------------
	///	�V�F�[�_�[
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPNTStaticInit)
	DECLARE_DX12SHADER(PSPNTStaticInit)

	//--------------------------------------------------------------------------------------
	///	�f�o�C�X�N���X
	//--------------------------------------------------------------------------------------
	class BaseDevice
	{
	public:
		//�t���[����
		static const UINT m_frameCount = 3;
		//���C�g��
		static const UINT m_numLights = 3;
		static const UINT m_numContexts = 1;

		static const int m_commandListCount = 3;
		static const int m_commandListPre = 0;
		static const int m_commandListMid = 1;
		static const int m_commandListPost = 2;

	private:
		//��
		UINT m_width;
		//����
		UINT m_height;
		//�c����
		float m_aspectRatio;
		//ESC�L�[�ŏI�������邩�ǂ���
		bool m_quiteEscapeKey;
		//���b�v���[�h���ǂ���
		bool m_useWarpDevice;
		//�E�C���h�E�^�C�g��
		wstring m_title;


		//�t���[���̃C���f�b�N�X
		UINT m_frameIndex;
		//�t�F���X�֘A
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
		UINT64 m_fenceValue;

		//�N���A����F
		Col4 m_clearColor;
		// �r���[�|�[�g�ƃV�U�[��`
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;


		//�p�C�v���C���I�u�W�F�N�g
		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		//�f�B�X�N�v���^�q�[�v
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;

		//�R�}���h�A���P�[�^�[
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		//�������p�̃R�}���h���X�g
		ComPtr<ID3D12GraphicsCommandList> m_initCommandList;

		//rtv�Ǘ��p
		UINT m_rtvDescriptorIncrementSize;
		//dsv�Ǘ��p
		UINT m_dsvDescriptorIncrementSize;
		//sampler�Ǘ��p
		map<wstring, UINT> m_samplerMap;
		UINT m_samplerDescriptorIncrementSize;
		const UINT m_samplerMax = 512;
		UINT m_samplerSendIndex;
		//cbvSrvUav�Ǘ��p
		UINT m_cbvSrvUavDescriptorIncrementSize;
		const UINT m_cbvSrvUavMax = 4096;
		const UINT m_srvStartIndex = 2;
		const UINT m_srvMax = 1024;
		UINT m_srvSendIndex;
		const UINT m_cbvUavStartIndex = 1024;
		const UINT m_cbvUavMax = 4096;
		UINT m_cbvUavSendIndex;

		//�A�v���P�[�V�������\�[�X
		//�^�C�}�[
		BaseTimer m_timer;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		//���[�g�V�O�l�`���Őݒ肳���GPU�X���b�g�̃}�b�v
		map<wstring, UINT> m_gpuSlotMap;

		ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
		ComPtr<ID3D12Resource> m_depthStencil;

		//�X�e�[�W���č\�z���ꂽ���ǂ���
		bool m_stageReCreated;

		//�t���[�����\�[�X
		BaseFrame* m_baseFrames[m_frameCount];
		BaseFrame* m_pCurrentBaseFrame;
		int m_currentBaseFrameIndex;

		//������enum
		enum class process {
			pipelineInit,
			update,
			begin,
			shadowmap,
			scene,
			mid,
			end
		};
		process m_process;
		//�p�C�v���C���I�u�W�F�N�g�̍쐬
		void LoadPipeline();
		//�A�Z�b�g�I�u�W�F�N�g�̍쐬
		void LoadAssets();
		//�n�[�h�E�F�A�A�_�v�^�[�𓾂�
		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		//���[�g�����̍쐬
		void CreateRootSignature();
		void CreatePipelineStates();
		void CreateRenderTargetViews();
		void CreateDepthStencil();
		void CreateSamplers();
		void CreateFrameResources();
		void CreateSynchronizationObjects();

		void BeginFrame();
		void MidFrame();
		void EndFrame();
		void WorkerThread();
		void SetCommonPipelineState(ID3D12GraphicsCommandList* pCommandList);

	public:

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	width	�Q�[����
		@param[in]	height	�Q�[������
		@param[in]	title	�E�C���h�E�^�C�g��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		BaseDevice(UINT width, UINT height,const wstring& title);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual ~BaseDevice();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[�����̎擾
		@return	�Q�[����
		*/
		//--------------------------------------------------------------------------------------
		UINT GetWidth() const { return m_width; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�Q�[�������̎擾
		@return	�Q�[������
		*/
		//--------------------------------------------------------------------------------------
		UINT GetHeight() const { return m_height; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�E�C���h�E�^�C�g���̎擾
		@return	�E�C���h�E�^�C�g��
		*/
		//--------------------------------------------------------------------------------------
		const wstring GetTitle() const { return m_title; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�r���[�|�[�g�̎擾
		@return	�r���[�|�[�g
		*/
		//--------------------------------------------------------------------------------------
		CD3DX12_VIEWPORT GetViewport() const { return m_viewport; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�[�U�[���N�g�̎擾
		@return	�V�[�U�[���N�g
		*/
		//--------------------------------------------------------------------------------------
		CD3DX12_RECT GetScissorRect() const { return m_scissorRect; }
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��ʂ��N���A����F�𓾂�
		@return	��ʂ��N���A����F
		*/
		//--------------------------------------------------------------------------------------
		Col4 GetClearColor() const {
			return m_clearColor;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	��ʂ��N���A����F��ݒ肷��
		@param[in]	params	���̃X�e�[�W���\�z����̂Ɏg�p����p�����[�^�B
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetClearColor(const Col4& col) {
			m_clearColor = col;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Esc�ŏI�����邩�ǂ����̎擾
		@return	�I������Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsQuiteEscapeKey() const {return m_quiteEscapeKey;}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Esc�ŏI�����邩�ǂ����̐ݒ�
		@param[in]	b	true/false
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetQuiteEscapeKey(bool b) {m_quiteEscapeKey = b;}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R�}���h���C�����p�[�X����
		@param[in]	argv	�R�}���h���C��������̃|�C���^�̔z��
		@param[in]	argc	�R�}���h���C���̕�����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X���b�v�`�F�[���̎擾
		@return	�X���b�v�`�F�[����ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<IDXGISwapChain3> GetSwapChain() const {
			return m_swapChain;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ID3D12Device�̎擾
		@return	ID3D12Device��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12Device> GetID3D12Device() const {
			return m_device;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Rtv�f�B�X�N���v�^�q�[�v�̎擾
		@return	Rtv�f�B�X�N���v�^�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() const {
			return m_rtvHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Dsv�f�B�X�N���v�^�q�[�v�̎擾
		@return	Dsv�f�B�X�N���v�^�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const {
			return m_dsvHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	CbvSrvUav�f�B�X�N���v�^�q�[�v�̎擾
		@return	CbvSrvUav�f�B�X�N���v�^�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetCbvSrvUavDescriptorHeap() const {
			return m_cbvSrvUavHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	CbvSrvUav�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		@return	CbvSrvUav�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y
		*/
		//--------------------------------------------------------------------------------------
		UINT GetCbvSrvUavDescriptorHandleIncrementSize() const {
			return m_cbvSrvUavDescriptorIncrementSize;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Srv�̎��̃C���f�b�N�X�̎擾
		@return	Srv�̎��̃C���f�b�N�X
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSrvNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Cbv�̎��̃C���f�b�N�X�̎擾
		@return	Cbv�̎��̃C���f�b�N�X
		*/
		//--------------------------------------------------------------------------------------
		UINT GetCbvUavNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�T���v���[�f�B�X�N���v�^�q�[�v�̎擾
		@return	�T���v���[�f�B�X�N���v�^�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetSamplerDescriptorHeap() const {
			return m_samplerHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�T���v���[�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		@return	�T���v���[�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerDescriptorHandleIncrementSize() const {
			return m_samplerDescriptorIncrementSize;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�T���v���[�̎��̃C���f�b�N�X�̎擾
		@return	�T���v���[�̎��̃C���f�b�N�X
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerNextIndex();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�w�肵���T���v���[�̃C���f�b�N�X�̎擾
		@param[in]	key	LinearWrap�Ȃǂ̃L�[
		@return	�w�肵���T���v���[�̃C���f�b�N�X�i���݂��Ȃ������炵����UINT_MAX��Ԃ��j
		*/
		//--------------------------------------------------------------------------------------
		UINT GetSamplerIndex(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���[�g�F�؂̎擾
		@return	���[�g�F�؂�ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12RootSignature> GetRootSignature() const {
			return m_rootSignature;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	GPU�̃X���b�gID�̐ݒ�
		@param[in]	key	s0,t0�Ȃǂ̃L�[
		@param[in]	val	ID
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetGpuSlot(const wstring& key, UINT val) {
			m_gpuSlotMap[key] = val;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	GPU�̃X���b�gID�̎擾
		@param[in]	key	s0,t0�Ȃǂ̃L�[
		@return	key�Ŏw�肵���L�[��ID
		*/
		//--------------------------------------------------------------------------------------
		UINT GetGpuSlotID(const wstring& key) {
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
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ElapsedTime�̎擾
		@return	ElapsedTime
		*/
		//--------------------------------------------------------------------------------------
		double GetElapsedTime() const {
			return m_timer.GetElapsedSeconds();
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���g�p���Ă���BaseFrame�̎擾
		@return	BaseFrame�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		BaseFrame* GetCurrentBaseFrame() const {
			return m_baseFrames[m_currentBaseFrameIndex];
		}

		BaseFrame** GetBaseFrames() {
			return m_baseFrames;
		}


		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�e�[�W���č\�z���ꂽ���ǂ���
		@return	�č\�z���ꂽ��true
		*/
		//--------------------------------------------------------------------------------------
		bool IsStageReCreated() const {
			return m_stageReCreated;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	G�X�e�[�W���č\�z���ꂽ���ǂ����ݒ�
		@param[in]	b�@�t���O
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetStageReCreated(bool b) {
			m_stageReCreated = b;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�ėp�I�ȃR�}���h���X�g�̎擾
		@return	�ėp�I�R�}���h���X�g��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12GraphicsCommandList> GetComandList()const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̏��������ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnCreate();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̍X�V�`�掞�ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnUpdateDraw();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̍X�V���ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnUpdate();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̕`�掞�ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnDraw();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̔j�����ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnDestroy();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�������ꂽ���ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnKeyDown(UINT8 key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�������ꂽ���ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnKeyUp(UINT8 key);
	};

}
// end namespace basecross
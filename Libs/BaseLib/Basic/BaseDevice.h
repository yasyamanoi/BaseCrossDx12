/*!
@file BaseDevice.h
@brief �f�o�C�X�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	�_�~�[�V�F�[�_�[
	//--------------------------------------------------------------------------------------
	DECLARE_DX12SHADER(VSPNTStaticInit)
	DECLARE_DX12SHADER(PSPNTStaticInit)

	struct BaseFrame;

	//--------------------------------------------------------------------------------------
	///	�f�o�C�X�N���X
	//--------------------------------------------------------------------------------------
	class BaseDevice
	{
		//��
		UINT m_width;
		//����
		UINT m_height;
		//�c����
		float m_aspectRatio;
		//ESC�L�[�ŏI�������邩�ǂ���
		bool m_QuiteEscapeKey;
		//���b�v���[�h���ǂ���
		bool m_useWarpDevice;
		//�E�C���h�E�^�C�g��
		wstring m_title;
		//�t���[����
		static const UINT m_frameCount = 3;

		//�N���A����F
		Col4 m_clearColor;

		// Pipeline objects.
		CD3DX12_VIEWPORT m_viewport;
		CD3DX12_RECT m_scissorRect;

		ComPtr<ID3D12Device> m_device;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;


		ComPtr<ID3D12RootSignature> m_rootSignature;
		ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
		ComPtr<ID3D12Resource> m_depthStencil;
		//�������p�̃p�C�v���C�X�e�[�g
		ComPtr<ID3D12PipelineState> m_initPipelineState;
		//�ėp�I�ȃR�}���h���X�g
		ComPtr<ID3D12GraphicsCommandList> m_initCommandList;

		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12PipelineState> m_pipelineStateShadowMap;

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
		const UINT m_srvStartIndex = 0;
		const UINT m_srvMax = 1024;
		UINT m_srvSendIndex;
		const UINT m_cbvUavStartIndex = 1024;
		const UINT m_cbvUavMax = 4096;
		UINT m_cbvUavSendIndex;

		//�����I�u�W�F�N�g
		ComPtr<ID3D12Fence> m_fence;
		UINT m_frameIndex;
		HANDLE m_fenceEvent;
		UINT64 m_fenceValue;

		//�������������I��������ǂ���
		bool m_isInited;

		//������enum
		enum class process {
			init,
			update,
			begin,
			shadowmap,
			render,
			end
		};
		process m_process;

		// �t���[��
		BaseFrame* m_baseFrame[m_frameCount];
		int m_currentBaseFrameIndex;

		//���[�g�V�O�l�`���Őݒ肳���GPU�X���b�g�̃}�b�v
		map<wstring, UINT> m_gpuSlotMap;

		//�A�v���P�[�V�������\�[�X
		BaseTimer m_timer;

		//�n�[�h�E�F�A�A�_�v�^�[�𓾂�
		void GetHardwareAdapter(
			_In_ IDXGIFactory1* pFactory,
			_Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		//�p�C�v���C���I�u�W�F�N�g�̍쐬
		void LoadPipeline();
		//�A�Z�b�g�I�u�W�F�N�g�̍쐬
		void LoadAssets();
		//���[�g�����̍쐬
		void CreateRootSignature();
		//�����_�����O�^�[�Q�b�g�r���[�̍쐬
		void CreateRenderTargetViews();
		//�f�v�X�X�e���V���r���[�̍쐬
		void CreateDepthStencilView();
		//�T���v���[�̍쐬
		void CreateSamplers();
		//�\�z�p�̃p�C�v���C���̍쐬
		void CreateInitPipelineState();
		//�t���[���̍쐬
		void CreateBaseFrame();
		//�R�}���h���X�g���W�߂�
		void PopulateCommandList(BaseFrame* pBaseFrame);
		//�����I�u�W�F�N�g�̍쐬
		void CreateSynchronizationObjects();
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
		@brief	�f�o�C�X�̏��������ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnInit();
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�o�C�X�̍X�V�`�掞�ɌĂ΂��C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void OnUpdateRender();
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
		void OnRender();
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
		Col4 GetClearColor() const{
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
		bool IsQuiteEscapeKey() const {return m_QuiteEscapeKey;}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Esc�ŏI�����邩�ǂ����̐ݒ�
		@param[in]	b	true/false
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetQuiteEscapeKey(bool b) {m_QuiteEscapeKey = b;}
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
		@brief	���[�g�F�؂̎擾
		@return	���[�g�F�؂�ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12RootSignature> GetRootSignature() const {
			return m_rootSignature;
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
		@brief	DSV�f�B�X�N���v�^�q�[�v�̎擾
		@return DSV�f�B�X�N���v�^�q�[�v��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() const {
			return m_dsvHeap;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	DSV�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y�̎擾
		@return	DSV�f�B�X�N���v�^�q�[�v�̃C���N�������g�T�C�Y
		*/
		//--------------------------------------------------------------------------------------
		UINT GetDsvDescriptorHandleIncrementSize() const {
			return m_dsvDescriptorIncrementSize;
		}
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
		@brief	�ėp�I�ȃR�}���h���X�g�̎擾
		@return	�ėp�I�R�}���h���X�g��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3D12GraphicsCommandList> GetComandList()const;
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
			return m_baseFrame[m_currentBaseFrameIndex];
		}
	};
}
// end namespace basecross
/*!
@file BaseScene.h
@brief �V�[���̐e�N���X
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {


	class Stage;

	//--------------------------------------------------------------------------------------
	///	�V�[�����L�̃C�x���g�\����
	//--------------------------------------------------------------------------------------
	struct SceneEvent {
		///	�x�����ԁiSendEvent�̏ꍇ�͏��0�j
		float m_dispatchTime;
		///	���b�Z�[�W������
		wstring m_msgStr;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�R���X�g���N�^
		@param[in]	dispatchTime	�z���܂ł̎���
		@param[in]	msgStr	���b�Z�[�W������
		*/
		//--------------------------------------------------------------------------------------
		SceneEvent(float dispatchTime, const wstring& msgStr) :
			m_dispatchTime(dispatchTime),
			m_msgStr(msgStr)
		{}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		~SceneEvent() {}
	};


	//--------------------------------------------------------------------------------------
	///	�V�[���e�N���X
	//--------------------------------------------------------------------------------------
	class BaseScene {
		map<wstring, shared_ptr<BaseMesh>> m_meshMap;
		map<wstring, shared_ptr<BaseTexture>> m_textureMap;
		shared_ptr<Stage> m_activeStage;
		vector<shared_ptr<SceneEvent>> m_eventVec;
		void CreateDefaultMeshes();
		void ConvertVertex(const vector<VertexPositionNormalTexture>& vertices,
			vector<VertexPositionColor>& new_pc_vertices,
			vector<VertexPositionNormal>& new_pn_vertices,
			vector<VertexPositionTexture>& new_pt_vertices,
			vector<VertexPositionNormalTangentTexture>& new_pntnt_vertices
		);
	protected:
		BaseScene():
			m_eventVec(),
			m_activeStage(nullptr)
		{}
		virtual ~BaseScene() {}
		void SetActiveStage(const shared_ptr<Stage>& stage) {
			m_activeStage = stage;
		}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u�ȃX�e�[�W��ݒ肵�ď���������
		@tparam	T	�擾����^�iStage�Ɍ^�ϊ��ł�����́j
		@tparam	Ts	�ϒ��ϐ��̌^
		@param[in]	params	���̃X�e�[�W���\�z����̂Ɏg�p����p�����[�^�B
		@return	�A�N�e�B�u�ȃX�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> ResetActiveStage(Ts&&... params) {
			auto ActStagePtr = GetActiveStage(false);
			if (ActStagePtr) {
				//�j����`����
				ActStagePtr->OnDestroy();
				ActStagePtr = nullptr;
				App::GetBaseDevice()->SetStageReCreated(true);
			}
			auto Ptr = ObjectFactory::Create<T>(params...);
			auto StagePtr = dynamic_pointer_cast<Stage>(Ptr);
			if (!StagePtr) {
				throw BaseException(
					L"�ȉ���Stage�Ɍ^�L���X�g�ł��܂���B",
					Util::GetWSTypeName<T>(),
					L"BaseScene::ResetActiveStage<T>()"
				);
			}
			SetActiveStage(StagePtr);
			return Ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u�ȃX�e�[�W��ݒ肵�ď���������
		@tparam	T	�擾����^�iStage�Ɍ^�ϊ��ł�����́j
		@tparam	Ts	�ϒ��ϐ��̌^
		@param[in]	params	���̃X�e�[�W���\�z����̂Ɏg�p����p�����[�^�B
		@return	�A�N�e�B�u�ȃX�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		shared_ptr<T> ResetActiveStageWithParam(Ts&&... params) {
			auto ActStagePtr = GetActiveStage(false);
			if (ActStagePtr) {
				//�j����`����
				ActStagePtr->DestroyStage();
			}
			auto Ptr = ObjectFactory::CreateWithParam<T>(params...);
			auto StagePtr = dynamic_pointer_cast<Stage>(Ptr);
			if (!StagePtr) {
				throw BaseException(
					L"�ȉ���Stage�Ɍ^�L���X�g�ł��܂���B",
					Util::GetWSTypeName<T>(),
					L"SceneBase::ResetActiveStageWithParam<T>()"
				);
			}
			SetActiveStage(StagePtr);
			//�f�o�b�O�p������
			auto dbgPtr = StagePtr->AddGameObject<DebugString>();
			StagePtr->SetSharedGameObject(L"DebugString", dbgPtr);
			return Ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u�ȃX�e�[�W���擾����
		@tparam	Ts	�ϒ��ϐ��̌^
		@param[in]	ExceptionActive	������F�߂邩�ǂ���
		@return	�A�N�e�B�u�ȃX�e�[�W
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<Stage> GetActiveStage(bool ExceptionActive = true) const;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���b�V�������\�[�X�o�^����
		@param[in]	key	���\�[�X�̃L�[
		@param[in]	mesh	���b�V��
		@param[in]	keyCheck	�L�[�̏d���`�F�b�N���邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RegisterMesh(const wstring& key, const shared_ptr<BaseMesh>& mesh, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�o�^����Ă��郁�b�V�����擾����
		@param[in]	key	���\�[�X�̃L�[
		@return	���b�V��
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseMesh> GetMesh(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�e�N�X�`�������\�[�X�o�^����
		@param[in]	key	���\�[�X�̃L�[
		@param[in]	texture	�e�N�X�`��
		@param[in]	keyCheck	�L�[�̏d���`�F�b�N���邩�ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void RegisterTexture(const wstring& key, const shared_ptr<BaseTexture>& texture, bool keyCheck = false);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�o�^����Ă���e�N�X�`�����擾����
		@param[in]	key	���\�[�X�̃L�[
		@return	�e�N�X�`��
		*/
		//--------------------------------------------------------------------------------------
		shared_ptr<BaseTexture> GetTexture(const wstring& key);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�V�[���ւ̃C�x���g��POST�i�L���[�ɓ����j
		@param[in]	dispatchTime	�x������
		@param[in]	msgStr	���b�Z�[�W
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void PostSceneEvent(float dispatchTime, const wstring& msgStr);
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g���󂯎��
		@param[in]	sceneEvent	�V�[���C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnEvent(const shared_ptr<SceneEvent>& sceneEvent) = 0;

		void DispatchDelayedEvent();


		virtual void OnPreCreate();
		virtual void OnCreate() = 0;
		virtual void OnInitFrame(BaseFrame* pBaseFrame);
		virtual void WriteConstantBuffers(BaseFrame* pBaseFrame);

		virtual void OnUpdate();
		virtual void OnDraw() {}
		virtual void OnDestroy();
		virtual void OnKeyDown(UINT8 key);
		virtual void OnKeyUp(UINT8 key);

		virtual void PopulateShadowmapCommandList(BaseFrame* pBaseFrame);
		virtual void PopulateCommandList(BaseFrame* pBaseFrame);

	};
}
// end namespace basecross
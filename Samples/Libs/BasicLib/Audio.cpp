/*!
@file Audio.h
@brief XAudio2を使ったオーディオクラス　実体
*/


#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	//	XAudio2Manager::Implクラス
	//--------------------------------------------------------------------------------------
	struct XAudio2Manager::Impl {
		bool m_IsAudioActive;
		ComPtr<IXAudio2>    m_IXAudio2;
		IXAudio2MasteringVoice* m_MasteringVoice;
		std::vector<std::shared_ptr<SoundItem>> m_SoundItemVec;
		Impl() :
			m_IsAudioActive(false),
			m_IXAudio2(nullptr),
			m_MasteringVoice(nullptr)
		{}
		~Impl() {
			XAUDIO2_VOICE_STATE state;
			bool isRunning;
			for (auto v : m_SoundItemVec) {
				if (v->m_SourceVoice) {
					v->m_SourceVoice->GetState(&state);
					isRunning = (state.BuffersQueued > 0) != 0;
					if (isRunning) {
						//			v->m_SourceVoice->Stop();
					}
					v->m_SourceVoice->DestroyVoice();
					v->m_SourceVoice = nullptr;
				}
			}
			m_IXAudio2.Reset();
			m_IXAudio2 = nullptr;
		}
		//アイテム配列をチェックしながら必要であれば追加する
		std::shared_ptr<SoundItem> ChkAndPushBackItem(const SoundItem& Item) {
			for (auto v : m_SoundItemVec) {
				if (!v->m_SourceVoice) {
					v->m_SourceVoice = Item.m_SourceVoice;
					v->m_AudioResource = Item.m_AudioResource;
					return v;
				}
			}
			auto Ptr = std::make_shared<SoundItem>();
			Ptr->m_SourceVoice = Item.m_SourceVoice;
			Ptr->m_AudioResource = Item.m_AudioResource;
			m_SoundItemVec.push_back(Ptr);
			return Ptr;
		}
		void OnUpdate() {
			XAUDIO2_VOICE_STATE state;
			bool isRunning;
			for (auto v : m_SoundItemVec) {
				if (v->m_SourceVoice) {
					v->m_SourceVoice->GetState(&state);
					isRunning = (state.BuffersQueued > 0) != 0;
					if (!isRunning) {
						v->m_AudioResource.reset();
						v->m_SourceVoice->DestroyVoice();
						v->m_SourceVoice = nullptr;
					}
				}
			}
		}
		bool IsItemActive(const std::shared_ptr<SoundItem>& ChkItem) {
			for (auto v : m_SoundItemVec) {
				if (v == ChkItem && v->m_SourceVoice) {
					return true;
				}
			}
			return false;
		}
	};



	//--------------------------------------------------------------------------------------
	/// XAudio2マネージャクラス
	//--------------------------------------------------------------------------------------
	XAudio2Manager::XAudio2Manager() :
		pImpl(new Impl())
	{}

	XAudio2Manager::~XAudio2Manager() {}

	bool XAudio2Manager::IsAudioActive() const {
		return IsCreated() && pImpl->m_IsAudioActive;
	}

	ComPtr<IXAudio2> XAudio2Manager::GetXAudio2() const {
		return pImpl->m_IXAudio2;
	}

	IXAudio2MasteringVoice* XAudio2Manager::GetMasteringVoice() const {
		return pImpl->m_MasteringVoice;
	}

	std::vector<std::shared_ptr<SoundItem>>& XAudio2Manager::GetSoundItemVec() {
		return pImpl->m_SoundItemVec;
	}
	const std::vector<std::shared_ptr<SoundItem>>& XAudio2Manager::GetSoundItemVec() const {
		return pImpl->m_SoundItemVec;
	}




	void XAudio2Manager::OnCreate() {
		UINT32 flags = 0;
		HRESULT hr = XAudio2Create(&pImpl->m_IXAudio2, flags);
		if (FAILED(hr)) {
			pImpl->m_IsAudioActive = false;
			return;
		}

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/) && defined(_DEBUG)
		// To see the trace output, you need to view ETW logs for this application:
		//    Go to Control Panel, Administrative Tools, Event Viewer.
		//    View->Show Analytic and Debug Logs.
		//    Applications and Services Logs / Microsoft / Windows / XAudio2. 
		//    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK 
		XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
		debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
		debug.BreakMask = XAUDIO2_LOG_ERRORS;
		pImpl->m_IXAudio2->SetDebugConfiguration(&debug, 0);
#endif
		//マスタリングボイス
		if (FAILED(hr = pImpl->m_IXAudio2->CreateMasteringVoice(&pImpl->m_MasteringVoice)))
		{
			pImpl->m_IXAudio2.Reset();
			pImpl->m_IsAudioActive = false;
			return;
		}
		pImpl->m_IsAudioActive = true;

	}

	void XAudio2Manager::OnUpdate(double elapsedTime) {
		if (!IsAudioActive()) {
			return;
		}
		pImpl->OnUpdate();
	}


	std::shared_ptr<SoundItem>  XAudio2Manager::Start(const std::wstring& ResKey, size_t LoopCount, float Volume) {
		if (!IsAudioActive()) {
			return nullptr;
		}
		auto SoundRes = Scene::Get()->GetAudio(ResKey);
		//ソースボイスの作成
		SoundItem Item;
		HRESULT hr = pImpl->m_IXAudio2->CreateSourceVoice(&Item.m_SourceVoice, SoundRes->GetOutputWaveFormatEx());
		if (FAILED(hr)) {
			pImpl->m_IsAudioActive = false;
			return nullptr;
		}
		Item.m_AudioResource = SoundRes;
		auto Ptr = pImpl->ChkAndPushBackItem(Item);
		XAUDIO2_BUFFER buffer = { 0 };
		buffer.AudioBytes = (UINT32)SoundRes->GetSoundData().size();
		buffer.LoopCount = (UINT32)LoopCount;
		buffer.pAudioData = &SoundRes->GetSoundData().front();
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		Ptr->m_SourceVoice->SetVolume(Volume);
		Ptr->m_SourceVoice->SubmitSourceBuffer(&buffer);
		Ptr->m_SourceVoice->Start();
		return Ptr;
	}

	void XAudio2Manager::Stop(const std::shared_ptr<SoundItem>& Item) {
		if (!IsAudioActive()) {
			return;
		}
		if (!Item) {
			return;
		}
		if (Item->m_SourceVoice) {
			XAUDIO2_VOICE_STATE state;
			bool isRunning;
			Item->m_SourceVoice->GetState(&state);
			isRunning = (state.BuffersQueued > 0) != 0;
			if (isRunning) {
				Item->m_SourceVoice->Stop();
			}
			Item->m_SourceVoice->DestroyVoice();
			Item->m_SourceVoice = nullptr;
		}
	}



	//--------------------------------------------------------------------------------------
	//	struct AudioResource::Impl;
	//	用途: Implイディオム
	//--------------------------------------------------------------------------------------
	struct AudioResource::Impl {
		WAVEFORMATEX m_WaveFormat;	//ウェブフォーマット
		std::vector<byte> m_SoundData;	//データ
		std::wstring m_FileName;		//ファイルへのパス
		Impl(const std::wstring& FileName) :
			m_FileName(FileName) {}
		~Impl() {}
		//ミューテックス
		std::mutex Mutex;

	};



	//--------------------------------------------------------------------------------------
	//	class AudioResource : public BaseResource;
	//	用途: オーディオリソース（wavなど）のラッピングクラス
	//--------------------------------------------------------------------------------------
	AudioResource::AudioResource(const std::wstring& FileName) :
		pImpl(new Impl(FileName))
	{
		try {

			if (!Scene::Get()->GetXAudio2Manager()->IsAudioActive()) {
				//マネージャが無効ならリターン
				return;
			}


			ThrowIfFailedEx(
				MFStartup(MF_VERSION),
				L"MediaFoundationの初期化に失敗しました",
				L"MFStartup(MF_VERSION)",
				L"AudioResource::AudioResource()"
			);

			ComPtr<IMFSourceReader> reader;

			ThrowIfFailedEx(
				MFCreateSourceReaderFromURL(FileName.c_str(), nullptr, &reader),
				L"サウンドリーダーの作成に失敗しました",
				L"MFCreateSourceReaderFromURL(FileName.c_str(),nullptr,&reader)",
				L"AudioResource::AudioResource()"
			);

			// Set the decoded output format as PCM.
			// XAudio2 on Windows can process PCM and ADPCM-encoded buffers.
			// When using MediaFoundation, this sample always decodes into PCM.
			Microsoft::WRL::ComPtr<IMFMediaType> mediaType;

			ThrowIfFailedEx(
				MFCreateMediaType(&mediaType),
				L"メディアタイプの作成に失敗しました",
				L"MFCreateMediaType(&mediaType)",
				L"AudioResource::AudioResource()"
			);

			ThrowIfFailedEx(
				mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio),
				L"メディアGUIDの設定に失敗しました",
				L"mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio)",
				L"AudioResource::AudioResource()"
			);

			ThrowIfFailedEx(
				mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM),
				L"メディアサブGUIDの設定に失敗しました",
				L"mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM)",
				L"AudioResource::AudioResource()"
			);

			ThrowIfFailedEx(
				reader->SetCurrentMediaType(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, mediaType.Get()),
				L"リーダーへのメディアタイプの設定に失敗しました",
				L"reader->SetCurrentMediaType(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, mediaType.Get())",
				L"AudioResource::AudioResource()"
			);


			// Get the complete WAVEFORMAT from the Media Type.
			Microsoft::WRL::ComPtr<IMFMediaType> outputMediaType;

			ThrowIfFailedEx(
				reader->GetCurrentMediaType(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &outputMediaType),
				L"出力用のメディアタイプの設定に失敗しました",
				L"reader->GetCurrentMediaType(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &outputMediaType)",
				L"AudioResource::AudioResource()"
			);

			UINT32 size = 0;
			WAVEFORMATEX* waveFormat;

			ThrowIfFailedEx(
				MFCreateWaveFormatExFromMFMediaType(outputMediaType.Get(), &waveFormat, &size),
				L"ウェブフォーマットの設定に失敗しました",
				L"MFCreateWaveFormatExFromMFMediaType(outputMediaType.Get(), &waveFormat, &size)",
				L"AudioResource::AudioResource()"
			);


			CopyMemory(&pImpl->m_WaveFormat, waveFormat, sizeof(pImpl->m_WaveFormat));
			CoTaskMemFree(waveFormat);

			PROPVARIANT propVariant;

			ThrowIfFailedEx(
				reader->GetPresentationAttribute(static_cast<uint32_t>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &propVariant),
				L"アトリビュートの設定に失敗しました",
				L"reader->GetPresentationAttribute(static_cast<uint32_t>(MF_SOURCE_READER_MEDIASOURCE), MF_PD_DURATION, &propVariant)",
				L"AudioResource::AudioResource()"
			);

			// 'duration' is in 100ns units; convert to seconds, and round up
			// to the nearest whole byte.
			LONGLONG duration = propVariant.uhVal.QuadPart;
			unsigned int maxStreamLengthInBytes =
				static_cast<unsigned int>(
					((duration * static_cast<ULONGLONG>(pImpl->m_WaveFormat.nAvgBytesPerSec)) + 10000000) /
					10000000
					);

			pImpl->m_SoundData.resize(maxStreamLengthInBytes);

			ComPtr<IMFSample> sample;
			ComPtr<IMFMediaBuffer> mediaBuffer;
			DWORD flags = 0;

			int positionInData = 0;
			bool done = false;
			while (!done)
			{

				ThrowIfFailedEx(
					reader->ReadSample(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &sample),
					L"サンプラーの読み込みに失敗しました",
					L"reader->ReadSample(static_cast<uint32_t>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, nullptr, &flags, nullptr, &sample)",
					L"AudioResource::AudioResource()"
				);

				if (sample != nullptr)
				{
					ThrowIfFailedEx(
						sample->ConvertToContiguousBuffer(&mediaBuffer),
						L"サンプラーのコンバートに失敗しました",
						L"sample->ConvertToContiguousBuffer(&mediaBuffer)",
						L"AudioResource::AudioResource()"
					);

					BYTE* audioData = nullptr;
					DWORD sampleBufferLength = 0;

					ThrowIfFailedEx(
						mediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength),
						L"バッファのLockに失敗しました",
						L"mediaBuffer->Lock(&audioData, nullptr, &sampleBufferLength)",
						L"AudioResource::AudioResource()"
					);

					for (DWORD i = 0; i < sampleBufferLength; i++)
					{
						pImpl->m_SoundData[positionInData++] = audioData[i];
					}
				}
				if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
				{
					done = true;
				}
			}
			int a = 10;
		}
		catch (...) {
			throw;
		}
	}
	AudioResource::~AudioResource() {}

	const std::vector<byte>& AudioResource::GetSoundData()const {
		return pImpl->m_SoundData;
	}

	WAVEFORMATEX* AudioResource::GetOutputWaveFormatEx()const {
		return &pImpl->m_WaveFormat;
	}




}
//end namespace basecross

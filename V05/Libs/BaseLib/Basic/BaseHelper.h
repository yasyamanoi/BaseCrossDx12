/*!
@file BaseHelper.h
@brief �w���p�[�N���X�A�֐��Q
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	/*!
	@brief	HRESULT�𕶎���ɕϊ�
	@param[in]	hr	HRESULT
	@return	������
	*/
	//--------------------------------------------------------------------------------------
	inline std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}

	//--------------------------------------------------------------------------------------
	///	��O�N���X
	//--------------------------------------------------------------------------------------
	class HrException : public std::runtime_error
	{
	public:
		HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
		HrException(HRESULT hr, const std::string& mess) :
			std::runtime_error(HrToString(hr) + mess),
			m_hr(hr)
		{}
		HRESULT Error() const { return m_hr; }
	private:
		const HRESULT m_hr;
	};

#define SAFE_RELEASE(p) if (p) (p)->Release()

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw HrException(hr);
		}
	}

	// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
	inline void SetName(ID3D12Object* pObject, LPCWSTR name)
	{
		pObject->SetName(name);
	}
	inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
	{
		WCHAR fullName[50];
		if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
		{
			pObject->SetName(fullName);
		}
	}
#else
	inline void SetName(ID3D12Object*, LPCWSTR)
	{
	}
	inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
	{
	}
#endif

	// ComPtr<T>�̂��߂̖��O���w���p�[

#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

	//--------------------------------------------------------------------------------------
	///	���[�e�B���e�B
	//--------------------------------------------------------------------------------------
	struct 	Util {
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�傫���������߂�.
		@tparam	T	��r����^
		@param[in]	v1	T�^�̒l1
		@param[in]	v2	T�^�̒l2
		@return	T�^�̑傫����
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T Maximum(const T& v1, const T& v2) {
			return v1 > v2 ? v1 : v2;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�������������߂�.
		@tparam	T	��r����^
		@param[in]	v1	T�^�̒l1
		@param[in]	v2	T�^�̒l2
		@return	T�^�̏�������
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T Minimum(const T& v1, const T& v2) {
			return v1 < v2 ? v1 : v2;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���S��COM�C���^�[�t�F�C�X�̃|�C���^���擾����w���p�[�֐�.
		@tparam	T	�擾����^
		@tparam	TCreateFunc	�쐬�֐��̌^
		@param[in,out]	comPtr	COM�|�C���^
		@param[in]	mutex	�~���[�e�b�N�X
		@param[in]	createFunc	�쐬�֐�
		@return	COM�������̏ꍇ�͐V�����쐬���āA����ȊO��COM���瓾��COM�C���^�[�t�F�C�X�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename TCreateFunc>
		static T* DemandCreate(ComPtr<T>& comPtr, std::mutex& mutex, TCreateFunc createFunc)
		{
			T* result = comPtr.Get();
			//���b�N��Ԃ��`�F�b�N
			MemoryBarrier();
			if (!result)
			{
				std::lock_guard<std::mutex> lock(mutex);
				result = comPtr.Get();
				if (!result)
				{
					createFunc(&result);
					MemoryBarrier();
					comPtr.Attach(result);
				}
			}
			return result;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���S��COM�C���^�[�t�F�C�X�̃|�C���^���擾����w���p�[�֐�.<br />
		�����̏ꍇ�͗�O���o��
		@tparam	T	�擾����^
		@param[in]	comPtr	COM�|�C���^
		@return�@COM�������̏ꍇ�͗�O���o���āA����ȊO��COM���瓾��COM�C���^�[�t�F�C�X�̃|�C���^
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T* GetComPtr(ComPtr<T>& comPtr) {
			T* result = comPtr.Get();
			//���b�N��Ԃ��`�F�b�N
			MemoryBarrier();
			if (!result)
			{
				//���s
				throw BaseException(
					L"����COM�C���^�[�t�F�C�X���擾�ł��܂���",
					Util::GetWSTypeName<T>(),
					L"Util::GetComPtr()"
				);
			}
			return result;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���[����󔒂���菜��<br/>
		���ɂȂ镶���񂩂��菜��
		@param[in,out]	wstr	���ƂɂȂ镶����i���^�[���ɂ��g�p�����j
		@param[in]	trimCharacterList = L" \t\v\r\n"	��菜������
		@return�@�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void WStrTrim(wstring& wstr, const wchar_t* TrimCharList = L" \t\v\r\n") {
			wstring result(L"");
			if (wstr.size() <= 0) {
				wstr = result;
				return;
			}
			wstring::size_type left = wstr.find_first_not_of(TrimCharList);
			if (left != wstring::npos) {
				wstring::size_type right = wstr.find_last_not_of(TrimCharList);
				result = wstr.substr(left, right - left + 1);
			}
			wstr = result;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�h�����񂩂�}���`�o�C�g������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in] src	�ϊ����镶����i���C�h�L�����j
		@return	�ϊ���̕�����i�}���`�o�C�g�j
		*/
		//--------------------------------------------------------------------------------------
		static std::string WStoRetMB(const wstring& src) {
			size_t i;
			char* pMBstr = new char[src.length() * MB_CUR_MAX + 1];
			wcstombs_s(
				&i,
				pMBstr,
				src.length() * MB_CUR_MAX + 1,
				src.c_str(),
				_TRUNCATE	//���ׂĕϊ��ł��Ȃ�������؂�̂�
			);
			string ret = pMBstr;
			delete[] pMBstr;
			return ret;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�}���`�o�C�g�����񂩂烏�C�h������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in]	src	�ϊ����镶����i�}���`�o�C�g�j
		@return	�ϊ���̕�����i���C�h������j
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring MBtoRetWS(const string& src) {
			size_t i;
			wchar_t* WCstr = new wchar_t[src.length() + 1];
			mbstowcs_s(
				&i,
				WCstr,
				src.length() + 1,
				src.c_str(),
				_TRUNCATE //���ׂĕϊ��ł��Ȃ�������؂�̂�
			);
			wstring ret = WCstr;
			delete[] WCstr;
			return ret;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�h�����񂩂�}���`�o�C�g������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in] src	�ϊ����镶����i���C�h�L�����j
		@param[out]	dest	�ϊ���̕�����i�}���`�o�C�g�j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void WStoMB(const wstring& src, string& dest) {
			size_t i;
			char* pMBstr = new char[src.length() * MB_CUR_MAX + 1];
			wcstombs_s(
				&i,
				pMBstr,
				src.length() * MB_CUR_MAX + 1,
				src.c_str(),
				_TRUNCATE	//���ׂĕϊ��ł��Ȃ�������؂�̂�
			);
			dest = pMBstr;
			delete[] pMBstr;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���C�h�����񂩂�}���`�o�C�gUTF8������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in]	src	�ϊ����镶����i���C�h�L�����j
		@param[out]	dest	�ϊ���̕�����i�}���`�o�C�g�j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void ConvertWstringtoUtf8(const wstring& src, string& dest) {
			INT bufsize = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
			char* Temp = new char[bufsize + 1];
			::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, Temp, bufsize, NULL, NULL);
			dest = Temp;
			delete[] Temp;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�}���`�o�C�gUTF8�������烏�C�h������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in]	src	�ϊ����镶����i�}���`�o�C�gUTF8�j
		@param[out]	dest	�ϊ��㕶����i���C�h�L�����j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void ConvertUtf8toWstring(const string& src, wstring& dest) {
			INT bufsize = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, (wchar_t*)NULL, 0);
			wchar_t* Temp = (wchar_t*)new wchar_t[bufsize];
			::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, Temp, bufsize);
			dest = Temp;
			delete[] Temp;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�}���`�o�C�g�����񂩂烏�C�h������ϊ�<br />
		���P�[���ˑ��̂��߁AWinMain()���ŁAsetlocale( LC_ALL, "JPN" );���K�v
		@param[in]	src	�ϊ����镶����i�}���`�o�C�g�j
		@param[out]	dest	�ϊ���̕�����i���C�h�L�����j
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void MBtoWS(const string& src, wstring& dest) {
			size_t i;
			wchar_t* WCstr = new wchar_t[src.length() + 1];
			mbstowcs_s(
				&i,
				WCstr,
				src.length() + 1,
				src.c_str(),
				_TRUNCATE //���ׂĕϊ��ł��Ȃ�������؂�̂�
			);
			dest = WCstr;
			delete[] WCstr;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	��������f���~�^�����ɂ���؂��Ĕz��ɕۑ�
		@param[out]	wstrvec	�ۑ������z��
		@param[in]	line	���ƂȂ镶����
		@param[in]	delimiter	�f���~�^
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		static void WStrToTokenVector(vector<wstring>& wstrvec, const wstring& line, wchar_t delimiter) {
			wstring::size_type i = 0;
			wstring wks(L"");
			for (i = 0; i < line.size(); i++) {
				if (line[i] == delimiter) {
					if (wks != L"") {
						wstrvec.push_back(wks);
					}
					wks = L"";
				}
				else {
					wks += line[i];
				}
			}
			wstrvec.push_back(wks);
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	T�^�̓����@��ʖ��𓾂�iwstring�j
		@tparam T	�^���𓾂�^
		@return	T�^�̓����@��ʖ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static wstring GetWSTypeName() {
			wstring clsname;
			MBtoWS(typeid(T).name(), clsname);
			return clsname;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	T�^�̓����@��ʖ��𓾂�istring�j
		@tparam T	�^���𓾂�^
		@return	T�^�̓����@��ʖ�
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static string GetMBTypeName() {
			string clsname = typeid(T).name();
			return clsname;
		}

		//--------------------------------------------------------------------------------------
		/// float�^�𕶎���ɕϊ�����ꍇ�̌`��
		//--------------------------------------------------------------------------------------
		enum FloatModify {
			Default = 0,	///< �f�t�H���g�i���������_�j
			Fixed,	///< �������o��
			Scientific,	///< e+09�Ȃǂ̌`��
		};

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���������_�𕶎���ɕϊ�����iwstring�Łj
		@param[in]	Val	���������_�l
		@param[in]	Precision = 0	���x0�Ńf�t�H���g
		@param[in]	Modify = FloatModify::Default	�\���`��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static wstring FloatToWStr(float Val, streamsize Precision = 0,
			FloatModify Modify = FloatModify::Default) {
			//�Ԃ�������
			wstring str;
			//�����𐮂���X�g���[��
			wostringstream stream;
			//���������_�̐��x�����߂�
			if (Precision > 0) {
				stream.precision(Precision);
			}
			switch (Modify) {
			case FloatModify::Fixed:
				stream << std::fixed << Val;
				break;
			case FloatModify::Scientific:
				stream << std::scientific << Val;
				break;
			case FloatModify::Default:
			default:
				stream << Val;
				break;
			}
			str = stream.str();
			return str;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	���������_�𕶎���ɕϊ�����istring�Łj
		@param[in]	Val	���������_�l
		@param[in]	Precision = 0	���x0�Ńf�t�H���g
		@param[in]	Modify = FloatModify::Default	�\���`��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static string FloatToStr(float Val, streamsize Precision = 0,
			FloatModify Modify = FloatModify::Default) {
			//�Ԃ�������
			string str;
			//�����𐮂���X�g���[��
			ostringstream stream;
			//���������_�̐��x�����߂�
			if (Precision > 0) {
				stream.precision(Precision);
			}
			switch (Modify) {
			case FloatModify::Fixed:
				stream << std::fixed << Val;
				break;
			case FloatModify::Scientific:
				stream << std::scientific << Val;
				break;
			case FloatModify::Default:
			default:
				stream << Val;
				break;
			}
			str = stream.str();
			return str;
		}


		//--------------------------------------------------------------------------------------
		/// �����^�𕶎���ɕϊ�����ꍇ�̌`��
		//--------------------------------------------------------------------------------------
		enum NumModify {
			Dec = 0,	///< 10�i��
			Hex,	///< 16�i��
			Oct,	///< 8�i��
		};
		//--------------------------------------------------------------------------------------
		/*!
		@brief	unsigned���l�𕶎���ɕϊ�����iwstring�Łj
		@param[in]	num	unsigned���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static wstring UintToWStr(UINT num, NumModify Modify = NumModify::Dec) {
			//�Ԃ�������
			wstring str;
			//�����𐮂���X�g���[��
			wostringstream stream;
			//�\���`�������߂�
			switch (Modify) {
			case NumModify::Oct:
				stream << std::oct << num;
				break;
			case NumModify::Hex:
				stream << std::hex << num;
				break;
			case NumModify::Dec:
			default:
				stream << std::dec << num;
				break;
			}
			str = stream.str();
			return str;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	size_t���l�𕶎���ɕϊ�����iwstring�Łj(UINT)�ɃL���X�g���邾���Ȃ̂ŁA32�r�b�g�𒴂���ꍇ�͒��ӂ��K�v
		@param[in]	num	unsigned���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static wstring SizeTToWStr(size_t num, NumModify Modify = NumModify::Dec) {
			//�Ԃ�������
			wstring str;
			//�����𐮂���X�g���[��
			wostringstream stream;
			//�\���`�������߂�
			switch (Modify) {
			case NumModify::Oct:
				stream << std::oct << num;
				break;
			case NumModify::Hex:
				stream << std::hex << num;
				break;
			case NumModify::Dec:
			default:
				stream << std::dec << num;
				break;
			}
			str = stream.str();
			return str;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	unsigned���l�𕶎���ɕϊ�����istring�Łj
		@param[in]	num	unsigned���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static string UintToStr(UINT num, NumModify Modify = NumModify::Dec) {
			//�Ԃ�������
			string str;
			//�����𐮂���X�g���[��
			ostringstream stream;
			//�\���`�������߂�
			switch (Modify) {
			case NumModify::Oct:
				stream << std::oct << num;
				break;
			case NumModify::Hex:
				stream << std::hex << num;
				break;
			case NumModify::Dec:
			default:
				stream << std::dec << num;
				break;
			}
			str = stream.str();
			return str;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	unsigned���l�𕶎���ɕϊ�����istring�Łj(UINT)�ɃL���X�g���邾���Ȃ̂ŁA32�r�b�g�𒴂���ꍇ�͒��ӂ��K�v
		@param[in]	num	unsigned���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static string SizeTToStr(UINT num, NumModify Modify = NumModify::Dec) {
			return UintToStr((UINT)num, Modify);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	signed���l�𕶎���ɕϊ�����iwstring�Łj
		@param[in]	num	signed���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static wstring IntToWStr(int num, NumModify Modify = NumModify::Dec) {
			//�Ԃ�������
			wstring str;
			//�����𐮂���X�g���[��
			wostringstream stream;
			//�\���`�������߂�
			switch (Modify) {
			case NumModify::Oct:
				stream << std::oct << num;
				break;
			case NumModify::Hex:
				stream << std::hex << num;
				break;
			case NumModify::Dec:
			default:
				stream << std::dec << num;
				break;
			}
			str = stream.str();
			return str;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	signed���l�𕶎���ɕϊ�����istring�Łj
		@param[in]	num	signed���l
		@param[in]	Modify = NumModify::Dec	�i��
		@return	�ϊ�����������
		*/
		//--------------------------------------------------------------------------------------
		static string IntToStr(int num, NumModify Modify = NumModify::Dec) {
			//�Ԃ�������
			string str;
			//�����𐮂���X�g���[��
			ostringstream stream;
			//�\���`�������߂�
			switch (Modify) {
			case NumModify::Oct:
				stream << std::oct << num;
				break;
			case NumModify::Hex:
				stream << std::hex << num;
				break;
			case NumModify::Dec:
			default:
				stream << std::dec << num;
				break;
			}
			str = stream.str();
			return str;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	Probability����1�̊m���ŁAtrue��Ԃ�<br />
		Probability��0���傫���Ȃ���΂Ȃ�Ȃ��i0���Z�͂ł��Ȃ��j
		@param[in]	Probability	�m���B������1�����w�肷��
		@return	Probability����1�̊m���ɂȂ��true
		*/
		//--------------------------------------------------------------------------------------
		static bool DivProbability(UINT Probability) {
			if (Probability <= 0) {
				return false;
			}
			double Prob = (double)rand() / (double)RAND_MAX;
			double Seed = 1.0 / (double)Probability;
			if (Prob <= Seed) {
				return true;
			}
			return false;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	0����1.0f�̊Ԃ̗�����Ԃ�<br />
		ZeroOK��false�̏ꍇ�A0���Ԃ�\���͂Ȃ��i0.00001f�ɂȂ�j
		@param[in]	ZeroOK = false	0���Ԃ��Ă������ǂ����̎w��
		@return	0����1.0f�̊Ԃ̗���
		*/
		//--------------------------------------------------------------------------------------
		static float RandZeroToOne(bool ZeroOK = false) {
			double Prob = (double)rand() / (double)RAND_MAX;
			if (Prob <= 0) {
				if (!ZeroOK) {
					Prob = 0.00001;
				}
				else {
					Prob = 0.0;
				}
			}
			return (float)Prob;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�؂�グ
		@param[in]	dSrc	���̐��l
		@param[in]	iLen	�����_�ȉ��̌���
		@return	������̐��l
		*/
		//--------------------------------------------------------------------------------------
		static double Ceil(double dSrc, int iLen)
		{
			double	dRet;

			dRet = dSrc * pow(10.0, iLen);
			dRet = (double)(int)(dRet + 0.9);

			return dRet * pow(10.0, -iLen);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�؂艺��
		@param[in]	dSrc	���̐��l
		@param[in]	iLen	�����_�ȉ��̌���
		@return	������̐��l
		*/
		//--------------------------------------------------------------------------------------
		static double Floor(double dSrc, int iLen)
		{
			double dRet;

			dRet = dSrc * pow(10.0, iLen);
			dRet = (double)(int)(dRet);

			return dRet * pow(10.0, -iLen);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�l�̌ܓ�
		@param[in]	dSrc	���̐��l
		@param[in]	iLen	�����_�ȉ��̌���
		@return	������̐��l
		*/
		//--------------------------------------------------------------------------------------
		static double Round(double dSrc, int iLen)
		{
			double	dRet;

			dRet = dSrc * pow(10.0, iLen);
			dRet = (double)(int)(dRet + 0.5);

			return dRet * pow(10.0, -iLen);
		}

	};


	//--------------------------------------------------------------------------------------
	/*!
	@brief	�J�X�^����ThrowIfFailed�iwstring�Łj
	@param[in]	hr	HRESULT
	@param[in]	wstr1	�G���[������1
	@param[in]	wstr2	�G���[������2
	@param[in]	wstr3	�G���[������3
	@return	�Ȃ��i����������킹��throw�j
	*/
	//--------------------------------------------------------------------------------------
	inline void ThrowIfFailed(HRESULT hr,
		const std::wstring& wstr1,
		const std::wstring& wstr2 = std::wstring(),
		const std::wstring& wstr3 = std::wstring()
	)
	{
		if (FAILED(hr))
		{
			string message = Util::WStoRetMB(wstr1);
			if (wstr2 != L"") {
				message += "\r\n";
				message += Util::WStoRetMB(wstr2);
			}
			if (wstr3 != L"") {
				message += "\r\n";
				message += Util::WStoRetMB(wstr3);
			}
			throw HrException(hr, message);
		}
	}

	//--------------------------------------------------------------------------------------
	/*!
	@brief	�J�X�^����ThrowIfFailed�istring�Łj
	@param[in]	hr	HRESULT
	@param[in]	str1	�G���[������1
	@param[in]	str2	�G���[������2
	@param[in]	str3	�G���[������3
	@return	�Ȃ��i����������킹��throw�j
	*/
	//--------------------------------------------------------------------------------------
	inline void ThrowIfFailed(HRESULT hr,
		const std::string& str1,
		const std::string& str2 = std::string(),
		const std::string& str3 = std::string()
	)
	{
		if (FAILED(hr))
		{
			string message = str1;
			if (str2 != "") {
				message += "\r\n";
				message += str2;
			}
			if (str3 != "") {
				message += "\r\n";
				message += str3;
			}
			throw HrException(hr, message);
		}
	}

	//--------------------------------------------------------------------------------------
	/// Base��O�N���X
	//--------------------------------------------------------------------------------------
	class BaseException : std::runtime_error
	{
		// ���b�Z�[�W�ϐ�
		string m_Message;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Base��O�̍쐬�iwstring�Łj
		@param[in]	m1	�G���[������1
		@param[in]	m2	�G���[������2
		@param[in]	m3	�G���[������3
		@return	�Ȃ��i����������킹�ĕێ��j
		*/
		//--------------------------------------------------------------------------------------
		BaseException(const wstring& m1, const wstring& m2 = wstring(L""), const wstring& m3 = wstring(L"")) :
			runtime_error("") {
			m_Message = Util::WStoRetMB(m1);
			if (m2 != L"") {
				m_Message += "\r\n";
				m_Message += Util::WStoRetMB(m2);
			}
			if (m3 != L"") {
				m_Message += "\r\n";
				m_Message += Util::WStoRetMB(m3);
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Base��O�̍쐬�istring�Łj
		@param[in]	m1	�G���[������1
		@param[in]	m2	�G���[������2
		@param[in]	m3	�G���[������3
		@return	�Ȃ��i����������킹�ĕێ��j
		*/
		//--------------------------------------------------------------------------------------
		BaseException(const string& m1, const string& m2 = string(""), const string& m3 = string("")) :
			runtime_error("") {
			m_Message = m1;
			if (m2 != "") {
				m_Message += "\r\n";
				m_Message += m2;
			}
			if (m3 != "") {
				m_Message += "\r\n";
				m_Message += m3;
			}
		}
		string what_m() const throw() {
			return m_Message;
		}
	};

	//--------------------------------------------------------------------------------------
	///	�p�t�H�[�}���X�J�E���^�[
	//--------------------------------------------------------------------------------------
	class PerformanceCounter {
		bool m_isActive;
		bool m_isStarted;
		float m_performanceTime;
		LARGE_INTEGER m_freq;
		LARGE_INTEGER m_before;
		LARGE_INTEGER m_after;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief �R���X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		PerformanceCounter() :
			m_isActive(false),
			m_performanceTime(0.0f),
			m_isStarted(false)
		{
			m_freq = {};
			m_before = {};
			m_after = {};
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �f�X�g���N�^
		*/
		//--------------------------------------------------------------------------------------
		~PerformanceCounter() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u���ǂ���
		@return	�A�N�e�B�u�Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsAvtive() const {
			return m_isActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�A�N�e�B�u���ǂ����ݒ�
		@param[in]	b	�A�N�e�B�u���ǂ���
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SetActive(bool b) {
			m_isActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���؂̊J�n
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void Start() {
			if (IsAvtive() && !m_isStarted) {
				// m_PerformanceTime��Start���͏��������Ȃ�
				// m_PerformanceTime = 0.0f;
				m_freq = {};
				m_before = {};
				m_after = {};
				m_isStarted = true;
				if (!QueryPerformanceFrequency(&m_freq))
				{
					throw BaseException(
						L"�V�X�e�����g�����擾�ł��܂���B",
						L"if (!QueryPerformanceFrequency(&m_freq))",
						L"PerformanceCounter::Start()"
					);
				}
				if (!QueryPerformanceCounter(&m_before))
				{
					throw BaseException(
						L"�p�t�H�[�}���X�J�E���^�i�����O�j���擾�ł��܂���B",
						L"if (!QueryPerformanceCounter(&m_before))",
						L"PerformanceCounter::Start()"
					);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���؂̏I��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void End() {
			if (IsAvtive() && m_isStarted) {
				if (!QueryPerformanceCounter(&m_after))
				{
					throw BaseException(
						L"�p�t�H�[�}���X�J�E���^�i������j���擾�ł��܂���B",
						L"if (!QueryPerformanceCounter(&m_after))",
						L"PerformanceCounter::Start()"
					);
				}
				if (m_freq.QuadPart != 0) {
					m_performanceTime = ((float)((m_after.QuadPart - m_before.QuadPart) * 1000.0f) / (float)m_freq.QuadPart);
				}
				m_isStarted = false;
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	���������p�t�H�[�}���X���ԁi�~���b�j��Ԃ�
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		float GetPerformanceTime() const {
			if (IsAvtive()) {
				return m_performanceTime;
			}
			else {
				return 0.0f;
			}
		}
	};


	class ObjectFactory;
	class ObjectInterface;



	//--------------------------------------------------------------------------------------
	///	Object�C���^�[�t�F�C�X
	//--------------------------------------------------------------------------------------
	class ObjectInterface : public std::enable_shared_from_this<ObjectInterface> {
		friend class ObjectFactory;
		//�N���G�C�g�ς݂��ǂ���
		//Create�֐����Ăяo�����true�ɂȂ�
		bool m_created{ false };
		void SetCreated(bool b) {
			m_created = b;
		}
	protected:
		ObjectInterface() {}
		virtual ~ObjectInterface() {}
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief this�|�C���^�ishared_ptr�j�̎擾
		@tparam T	�|�C���^�̌^
		@return�@�I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		std::shared_ptr<T> GetThis() {
			auto ptr = dynamic_pointer_cast<T>(shared_from_this());
			if (ptr) {
				return ptr;
			}
			else {
				wstring str(L"this��");
				str += Util::GetWSTypeName<T>();
				str += L"�^�ɃL���X�g�ł��܂���";
				throw BaseException(
					str,
					L"if( ! dynamic_pointer_cast<T>(shared_from_this()) )",
					L"ObjectInterface::GetThis()"
				);
			}
			return nullptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �N���G�C�g�ς݂����擾
		@return�@�N���G�C�g�ς݂Ȃ�true
		*/
		//--------------------------------------------------------------------------------------
		bool IsCreated()const {
			return m_created;
		}
		//���z�֐��Q
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�������O����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnPreCreate() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	����������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnCreate() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�X�V�����Q
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnUpdate2() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�`�揈��
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDraw() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�j��������
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnDestroy() = 0;
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�{�[�h�����ꂽ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyDown(UINT8 /*key*/) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�L�[�{�[�h�����ꂽ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnKeyUp(UINT8 /*key*/) {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��POST�i�L���[�ɓ����j
		@param[in]	DispatchTime	POST���鎞�ԁi0�Ŏ��̃^�[���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	Receiver	�C�x���g��M�ҁinullptr�s�j
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��POST�i�L���[�ɓ����j
		@param[in]	DispatchTime	POST���鎞�ԁi0�Ŏ��̃^�[���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	ReceiverKey	�󂯎葤�I�u�W�F�N�g�𔻕ʂ���L�[
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void PostEvent(float DispatchTime, const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());

		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��SEND�i�L���[�ɓ��ꂸ�ɂ��̂܂ܑ���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	ReceiverKey	�󂯎葤�I�u�W�F�N�g�𔻕ʂ���L�[
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const shared_ptr<ObjectInterface>& Receiver,
			const wstring& MsgStr, const shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g��SEND�i�L���[�ɓ��ꂸ�ɂ��̂܂ܑ���j
		@param[in]	Sender	�C�x���g���M�ҁinullptr�j
		@param[in]	Receiver	�C�x���g��M�ҁinullptr�s�j
		@param[in]	MsgStr	���b�Z�[�W
		@param[in,out]	Info	�ǉ����
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		void SendEvent(const shared_ptr<ObjectInterface>& Sender, const wstring& ReceiverKey,
			const wstring& MsgStr, const  shared_ptr<void>& Info = shared_ptr<void>());
		//--------------------------------------------------------------------------------------
		/*!
		@brief	�C�x���g���󂯎��
		@param[in]	event	�C�x���g
		@return	�Ȃ�
		*/
		//--------------------------------------------------------------------------------------
		virtual void OnEvent(const shared_ptr<Event>& event) {}

	private:
		//�R�s�[�֎~
		ObjectInterface(const ObjectInterface&) = delete;
		ObjectInterface& operator=(const ObjectInterface&) = delete;
		//���[�u�֎~
		ObjectInterface(const ObjectInterface&&) = delete;
		ObjectInterface& operator=(const ObjectInterface&&) = delete;
	};


	//--------------------------------------------------------------------------------------
	///	Object���\�z����
	//--------------------------------------------------------------------------------------
	class ObjectFactory {
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief �I�u�W�F�N�g�쐬�istatic�֐��j
		@tparam T	�쐬����^
		@tparam Ts...	�ϒ��p�����[�^�^
		@param[in]	params	�ϒ��p�����[�^
		@return�@�쐬�����I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static shared_ptr<T> Create(Ts&&... params) {
			shared_ptr<T> ptr = shared_ptr<T>(new T(params...));
			//�������֐��Ăяo��
			ptr->OnPreCreate();
			ptr->OnCreate();
			ptr->SetCreated(true);
			return ptr;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �I�u�W�F�N�g�쐬�istatic�֐��j�B�p�����[�^��OnInit�ɓn�����
		@tparam T	�쐬����^
		@tparam Ts...	�ϒ��p�����[�^�^
		@param[in]	params	�ϒ��p�����[�^
		@return�@�쐬�����I�u�W�F�N�g��shared_ptr
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename... Ts>
		static shared_ptr<T> CreateInitParam(Ts&&... params) {
			shared_ptr<T> ptr = shared_ptr<T>(new T());
			//�������֐��Ăяo��
			ptr->OnCreate(params...);
			ptr->SetCreated(true);
			return ptr;
		}

	};


	//--------------------------------------------------------------------------------------
	///	�V�F�[�_�֘A���\�[�X�̃C���^�[�t�F�C�X
	//--------------------------------------------------------------------------------------
	class Dx12ShaderResource {
	protected:
		Dx12ShaderResource();
		virtual ~Dx12ShaderResource();
		//�V�F�[�_�A�N�Z�T
		ID3DBlob* GetShaderBlob(const wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr);
		ComPtr<ID3DBlob>& GetShaderBlobComPtr(const wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr);
	private:
		//�R�s�[�֎~
		Dx12ShaderResource(const Dx12ShaderResource&) = delete;
		Dx12ShaderResource& operator=(const Dx12ShaderResource&) = delete;
		//���[�u�֎~
		Dx12ShaderResource(const Dx12ShaderResource&&) = delete;
		Dx12ShaderResource& operator=(const Dx12ShaderResource&&) = delete;
	};

	//--------------------------------------------------------------------------------------
	///	�V���O���g���V�F�[�_�̐e�N���X(�V�F�[�_�̎�ނɂ�����炸�����N���X�g�p)
	//--------------------------------------------------------------------------------------
	template<typename ShaderType>
	class Dx12Shader : public Dx12ShaderResource {
		wstring m_fileName;
	protected:
		//�f���[�^�[
		struct Deleter
		{
			void operator()(ShaderType* p) { delete p; }
		};
		ComPtr<ID3DBlob> m_shaderPtr;
		//�\�z�Ɣj��
		Dx12Shader<ShaderType>(const wstring& fileName) : Dx12ShaderResource(), m_fileName(fileName) {}
		virtual ~Dx12Shader() {}
		static unique_ptr<ShaderType, Deleter> m_ptr;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief �V�F�[�_�[��blob�擾
		@return�@�V�F�[�_�[��blob
		*/
		//--------------------------------------------------------------------------------------
		ID3DBlob* GetShader() {
			return GetShaderBlob(m_fileName, m_shaderPtr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �V�F�[�_�[��ComPtr�擾
		@return�@�V�F�[�_�[��ComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3DBlob>& GetShaderComPtr() {
			return GetShaderBlobComPtr(m_fileName, m_shaderPtr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief �V�F�[�_�[�̃C���X�^���X�擾
		@return�@�V�F�[�_�[�̃C���X�^���X
		*/
		//--------------------------------------------------------------------------------------
		static unique_ptr<ShaderType, Deleter>& GetPtr() {
			if (!m_ptr) {
				m_ptr.reset(new ShaderType());
			}
			return m_ptr;
		}
	};


	//������ʃV�F�[�_�[
	//�V�F�[�_�錾�p�}�N��
#define DECLARE_DX12SHADER(ShaderName) class ShaderName : public Dx12Shader<ShaderName>{ \
	public: \
		ShaderName(); \
	};
	//�V�F�[�_���̗p�}�N��
#define IMPLEMENT_DX12SHADER(ShaderName,CsoFilename) unique_ptr<ShaderName, ShaderName::Deleter> ShaderName::m_ptr; \
	ShaderName::ShaderName() : \
	Dx12Shader(CsoFilename){}


}
// end namespace basecross
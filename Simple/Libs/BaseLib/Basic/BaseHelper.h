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
@file BaseHelper.h
@brief ユーティリティクラス、関数群
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#pragma once

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ユーティリティ
	//--------------------------------------------------------------------------------------
	struct 	Util {
		//--------------------------------------------------------------------------------------
		/*!
		@brief	大きい方を求める.
		@tparam	T	比較する型
		@param[in]	v1	T型の値1
		@param[in]	v2	T型の値2
		@return	T型の大きい方
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T Maximum(const T& v1, const T& v2) {
			return v1 > v2 ? v1 : v2;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	小さい方を求める.
		@tparam	T	比較する型
		@param[in]	v1	T型の値1
		@param[in]	v2	T型の値2
		@return	T型の小さい方
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T Minimum(const T& v1, const T& v2) {
			return v1 < v2 ? v1 : v2;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	安全にCOMインターフェイスのポインタを取得するヘルパー関数.
		@tparam	T	取得する型
		@tparam	TCreateFunc	作成関数の型
		@param[in,out]	comPtr	COMポインタ
		@param[in]	mutex	ミューテックス
		@param[in]	createFunc	作成関数
		@return	COMが無効の場合は新しく作成して、それ以外はCOMから得たCOMインターフェイスのポインタ
		*/
		//--------------------------------------------------------------------------------------
		template<typename T, typename TCreateFunc>
		static T* DemandCreate(ComPtr<T>& comPtr, std::mutex& mutex, TCreateFunc createFunc)
		{
			T* result = comPtr.Get();
			//ロック状態をチェック
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
		@brief	安全にCOMインターフェイスのポインタを取得するヘルパー関数.<br />
		無効の場合は例外を出す
		@tparam	T	取得する型
		@param[in]	comPtr	COMポインタ
		@return　COMが無効の場合は例外を出して、それ以外はCOMから得たCOMインターフェイスのポインタ
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static T* GetComPtr(ComPtr<T>& comPtr) {
			T* result = comPtr.Get();
			//ロック状態をチェック
			MemoryBarrier();
			if (!result)
			{
				//失敗
				throw BaseException(
					L"このCOMインターフェイスを取得できません",
					Util::GetWSTypeName<T>(),
					L"Util::GetComPtr()"
				);
			}
			return result;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	両端から空白を取り除く<br/>
		元になる文字列から取り除く
		@param[in,out]	wstr	もとになる文字列（リターンにも使用される）
		@param[in]	trimCharacterList = L" \t\v\r\n"	取り除く文字
		@return　なし
		*/
		//--------------------------------------------------------------------------------------
		static void WStrTrim(std::wstring& wstr, const wchar_t* TrimCharList = L" \t\v\r\n") {
			std::wstring result(L"");
			if (wstr.size() <= 0) {
				wstr = result;
				return;
			}
			std::wstring::size_type left = wstr.find_first_not_of(TrimCharList);
			if (left != std::wstring::npos) {
				std::wstring::size_type right = wstr.find_last_not_of(TrimCharList);
				result = wstr.substr(left, right - left + 1);
			}
			wstr = result;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ワイド文字列からマルチバイト文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in] src	変換する文字列（ワイドキャラ）
		@return	変換後の文字列（マルチバイト）
		*/
		//--------------------------------------------------------------------------------------
		static std::string WStoRetMB(const std::wstring& src) {
			size_t i;
			char* pMBstr = new char[src.length() * MB_CUR_MAX + 1];
			wcstombs_s(
				&i,
				pMBstr,
				src.length() * MB_CUR_MAX + 1,
				src.c_str(),
				_TRUNCATE	//すべて変換できなかったら切り捨て
			);
			std::string ret = pMBstr;
			delete[] pMBstr;
			return ret;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	マルチバイト文字列からワイド文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in]	src	変換する文字列（マルチバイト）
		@return	変換後の文字列（ワイド文字列）
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring MBtoRetWS(const std::string& src) {
			size_t i;
			wchar_t* WCstr = new wchar_t[src.length() + 1];
			mbstowcs_s(
				&i,
				WCstr,
				src.length() + 1,
				src.c_str(),
				_TRUNCATE //すべて変換できなかったら切り捨て
			);
			std::wstring ret = WCstr;
			delete[] WCstr;
			return ret;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	ワイド文字列からマルチバイト文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in] src	変換する文字列（ワイドキャラ）
		@param[out]	dest	変換後の文字列（マルチバイト）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void WStoMB(const std::wstring& src, std::string& dest) {
			size_t i;
			char* pMBstr = new char[src.length() * MB_CUR_MAX + 1];
			wcstombs_s(
				&i,
				pMBstr,
				src.length() * MB_CUR_MAX + 1,
				src.c_str(),
				_TRUNCATE	//すべて変換できなかったら切り捨て
			);
			dest = pMBstr;
			delete[] pMBstr;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	ワイド文字列からマルチバイトUTF8文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in]	src	変換する文字列（ワイドキャラ）
		@param[out]	dest	変換後の文字列（マルチバイト）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void ConvertWstringtoUtf8(const std::wstring& src, std::string& dest) {
			INT bufsize = ::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, NULL, 0, NULL, NULL);
			char* Temp = new char[bufsize + 1];
			::WideCharToMultiByte(CP_UTF8, 0, src.c_str(), -1, Temp, bufsize, NULL, NULL);
			dest = Temp;
			delete[] Temp;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	マルチバイトUTF8文字からワイド文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in]	src	変換する文字列（マルチバイトUTF8）
		@param[out]	dest	変換後文字列（ワイドキャラ）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void ConvertUtf8toWstring(const std::string& src, std::wstring& dest) {
			INT bufsize = ::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, (wchar_t*)NULL, 0);
			wchar_t* Temp = (wchar_t*)new wchar_t[bufsize];
			::MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, Temp, bufsize);
			dest = Temp;
			delete[] Temp;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	マルチバイト文字列からワイド文字列変換<br />
		ロケール依存のため、WinMain()等で、setlocale( LC_ALL, "JPN" );が必要
		@param[in]	src	変換する文字列（マルチバイト）
		@param[out]	dest	変換後の文字列（ワイドキャラ）
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void MBtoWS(const std::string& src, std::wstring& dest) {
			size_t i;
			wchar_t* WCstr = new wchar_t[src.length() + 1];
			mbstowcs_s(
				&i,
				WCstr,
				src.length() + 1,
				src.c_str(),
				_TRUNCATE //すべて変換できなかったら切り捨て
			);
			dest = WCstr;
			delete[] WCstr;
		}

		//--------------------------------------------------------------------------------------
		/*!
		@brief	文字列をデリミタ文字により区切って配列に保存
		@param[out]	wstrvec	保存される配列
		@param[in]	line	元となる文字列
		@param[in]	delimiter	デリミタ
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		static void WStrToTokenVector(std::vector<std::wstring>& wstrvec, const std::wstring& line, wchar_t delimiter) {
			std::wstring::size_type i = 0;
			std::wstring wks(L"");
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
		@brief	T型の内部機器別名を得る（wstring）
		@tparam T	型名を得る型
		@return	T型の内部機器別名
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static std::wstring GetWSTypeName() {
			std::wstring clsname;
			MBtoWS(typeid(T).name(), clsname);
			return clsname;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	T型の内部機器別名を得る（string）
		@tparam T	型名を得る型
		@return	T型の内部機器別名
		*/
		//--------------------------------------------------------------------------------------
		template<typename T>
		static std::string GetMBTypeName() {
			std::string clsname = typeid(T).name();
			return clsname;
		}

		//--------------------------------------------------------------------------------------
		/// float型を文字列に変換する場合の形式
		//--------------------------------------------------------------------------------------
		enum class FloatModify {
			Default = 0,	///< デフォルト（浮動小数点）
			Fixed,	///< 数字を出力
			Scientific,	///< e+09などの形式
			Count ///<カウント
		};

		//--------------------------------------------------------------------------------------
		/*!
		@brief	浮動小数点を文字列に変換する（wstring版）
		@param[in]	Val	浮動小数点値
		@param[in]	Precision = 0	精度0でデフォルト
		@param[in]	Modify = FloatModify::Default	表示形式
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring FloatToWStr(float Val, std::streamsize Precision = 0,
			FloatModify Modify = FloatModify::Default) {
			//返す文字列
			std::wstring str;
			//書式を整えるストリーム
			std::wostringstream stream;
			//浮動小数点の精度を決める
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
		@brief	浮動小数点を文字列に変換する（string版）
		@param[in]	Val	浮動小数点値
		@param[in]	Precision = 0	精度0でデフォルト
		@param[in]	Modify = FloatModify::Default	表示形式
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::string FloatToStr(float Val, std::streamsize Precision = 0,
			FloatModify Modify = FloatModify::Default) {
			//返す文字列
			std::string str;
			//書式を整えるストリーム
			std::ostringstream stream;
			//浮動小数点の精度を決める
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
		/// 整数型を文字列に変換する場合の形式
		//--------------------------------------------------------------------------------------
		enum class NumModify {
			Dec = 0,	///< 10進数
			Hex,	///< 16進数
			Oct,	///< 8進数
			Count ///<カウント
		};
		//--------------------------------------------------------------------------------------
		/*!
		@brief	unsigned数値を文字列に変換する（wstring版）
		@param[in]	num	unsigned数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring UintToWStr(UINT num, NumModify Modify = NumModify::Dec) {
			//返す文字列
			std::wstring str;
			//書式を整えるストリーム
			std::wostringstream stream;
			//表示形式を決める
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
		@brief	size_t数値を文字列に変換する（wstring版）(UINT)にキャストするだけなので、32ビットを超える場合は注意が必要
		@param[in]	num	unsigned数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring SizeTToWStr(size_t num, NumModify Modify = NumModify::Dec) {
			//返す文字列
			std::wstring str;
			//書式を整えるストリーム
			std::wostringstream stream;
			//表示形式を決める
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
		@brief	unsigned数値を文字列に変換する（string版）
		@param[in]	num	unsigned数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::string UintToStr(UINT num, NumModify Modify = NumModify::Dec) {
			//返す文字列
			std::string str;
			//書式を整えるストリーム
			std::ostringstream stream;
			//表示形式を決める
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
		@brief	unsigned数値を文字列に変換する（string版）(UINT)にキャストするだけなので、32ビットを超える場合は注意が必要
		@param[in]	num	unsigned数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::string SizeTToStr(UINT num, NumModify Modify = NumModify::Dec) {
			return UintToStr((UINT)num, Modify);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	signed数値を文字列に変換する（wstring版）
		@param[in]	num	signed数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::wstring IntToWStr(int num, NumModify Modify = NumModify::Dec) {
			//返す文字列
			std::wstring str;
			//書式を整えるストリーム
			std::wostringstream stream;
			//表示形式を決める
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
		@brief	signed数値を文字列に変換する（string版）
		@param[in]	num	signed数値
		@param[in]	Modify = NumModify::Dec	進数
		@return	変換した文字列
		*/
		//--------------------------------------------------------------------------------------
		static std::string IntToStr(int num, NumModify Modify = NumModify::Dec) {
			//返す文字列
			std::string str;
			//書式を整えるストリーム
			std::ostringstream stream;
			//表示形式を決める
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
		@brief	Probability分の1の確率で、trueを返す<br />
		Probabilityは0より大きくなければならない（0除算はできない）
		@param[in]	Probability	確率。何分の1かを指定する
		@return	Probability分の1の確率になればtrue
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
		@brief	0から1.0fの間の乱数を返す<br />
		ZeroOKがfalseの場合、0が返る可能性はない（0.00001fになる）
		@param[in]	ZeroOK = false	0が返っていいかどうかの指定
		@return	0から1.0fの間の乱数
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
		@brief	切り上げ
		@param[in]	dSrc	元の数値
		@param[in]	iLen	小数点以下の桁数
		@return	処理後の数値
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
		@brief	切り下げ
		@param[in]	dSrc	元の数値
		@param[in]	iLen	小数点以下の桁数
		@return	処理後の数値
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
		@brief	四捨五入
		@param[in]	dSrc	元の数値
		@param[in]	iLen	小数点以下の桁数
		@return	処理後の数値
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
	@brief	カスタム版ThrowIfFailed（wstring版）
	@param[in]	hr	HRESULT
	@param[in]	wstr1	エラー文字列1
	@param[in]	wstr2	エラー文字列2
	@param[in]	wstr3	エラー文字列3
	@return	なし（文字列をあわせてthrow）
	*/
	//--------------------------------------------------------------------------------------
	inline void ThrowIfFailedEx(HRESULT hr,
		const std::wstring& wstr1,
		const std::wstring& wstr2 = std::wstring(),
		const std::wstring& wstr3 = std::wstring()
	)
	{
		if (FAILED(hr))
		{
			std::string message = "\r\n";
			message += Util::WStoRetMB(wstr1);
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
	@brief	カスタム版ThrowIfFailed（string版）
	@param[in]	hr	HRESULT
	@param[in]	str1	エラー文字列1
	@param[in]	str2	エラー文字列2
	@param[in]	str3	エラー文字列3
	@return	なし（文字列をあわせてthrow）
	*/
	//--------------------------------------------------------------------------------------
	inline void ThrowIfFailedEx(HRESULT hr,
		const std::string& str1,
		const std::string& str2 = std::string(),
		const std::string& str3 = std::string()
	)
	{
		if (FAILED(hr))
		{
			std::string message = str1;
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
	/// Base例外クラス
	//--------------------------------------------------------------------------------------
	class BaseException : std::runtime_error
	{
		// メッセージ変数
		std::string m_Message;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief	Base例外の作成（wstring版）
		@param[in]	m1	エラー文字列1
		@param[in]	m2	エラー文字列2
		@param[in]	m3	エラー文字列3
		@return	なし（文字列をあわせて保持）
		*/
		//--------------------------------------------------------------------------------------
		BaseException(const std::wstring& m1, const std::wstring& m2 = std::wstring(L""), const std::wstring& m3 = std::wstring(L"")) :
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
		@brief	Base例外の作成（string版）
		@param[in]	m1	エラー文字列1
		@param[in]	m2	エラー文字列2
		@param[in]	m3	エラー文字列3
		@return	なし（文字列をあわせて保持）
		*/
		//--------------------------------------------------------------------------------------
		BaseException(const std::string& m1, const std::string& m2 = std::string(""), const std::string& m3 = std::string("")) :
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
		std::string what_m() const throw() {
			return m_Message;
		}
	};

	//--------------------------------------------------------------------------------------
	///	パフォーマンスカウンター
	//--------------------------------------------------------------------------------------
	class PerformanceCounter {
		bool m_isActive;
		bool m_isStarted;
		float m_performanceTime;
		LARGE_INTEGER m_freq;
		LARGE_INTEGER m_before;
		LARGE_INTEGER m_after;
	public:
		PerformanceCounter() :
			m_isActive(false),
			m_performanceTime(0.0f),
			m_isStarted(false)
		{
			m_freq = {};
			m_before = {};
			m_after = {};
		}
		~PerformanceCounter() {}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブかどうか
		@return	アクティブならtrue
		*/
		//--------------------------------------------------------------------------------------
		bool IsAvtive() const {
			return m_isActive;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	アクティブかどうか設定
		@param[in]	b	アクティブかどうか
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void SetActive(bool b) {
			m_isActive = b;
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	検証の開始
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void Start() {
			if (IsAvtive() && !m_isStarted) {
				// m_PerformanceTimeはStart時は初期化しない
				// m_PerformanceTime = 0.0f;
				m_freq = {};
				m_before = {};
				m_after = {};
				m_isStarted = true;
				if (!QueryPerformanceFrequency(&m_freq))
				{
					throw BaseException(
						L"システム周波数を取得できません。",
						L"if (!QueryPerformanceFrequency(&m_freq))",
						L"PerformanceCounter::Start()"
					);
				}
				if (!QueryPerformanceCounter(&m_before))
				{
					throw BaseException(
						L"パフォーマンスカウンタ（処理前）を取得できません。",
						L"if (!QueryPerformanceCounter(&m_before))",
						L"PerformanceCounter::Start()"
					);
				}
			}
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief	検証の終了
		@return	なし
		*/
		//--------------------------------------------------------------------------------------
		void End() {
			if (IsAvtive() && m_isStarted) {
				if (!QueryPerformanceCounter(&m_after))
				{
					throw BaseException(
						L"パフォーマンスカウンタ（処理後）を取得できません。",
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
		@brief	かかったパフォーマンス時間（ミリ秒）を返す
		@return	なし
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

	//--------------------------------------------------------------------------------------
	///	シェーダ関連リソースのインターフェイス
	//--------------------------------------------------------------------------------------
	class Dx12ShaderResource {
	protected:
		Dx12ShaderResource();
		virtual ~Dx12ShaderResource();
		//シェーダアクセサ
		ID3DBlob* GetShaderBlob(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr);
		ComPtr<ID3DBlob>& GetShaderBlobComPtr(const std::wstring& fileName, ComPtr<ID3DBlob>& shaderComPtr);
	private:
		//コピー禁止
		Dx12ShaderResource(const Dx12ShaderResource&) = delete;
		Dx12ShaderResource& operator=(const Dx12ShaderResource&) = delete;
		//ムーブ禁止
		Dx12ShaderResource(const Dx12ShaderResource&&) = delete;
		Dx12ShaderResource& operator=(const Dx12ShaderResource&&) = delete;
	};

	//--------------------------------------------------------------------------------------
	///	シングルトンシェーダの親クラス(シェーダの種類にかかわらず同じクラス使用)
	//--------------------------------------------------------------------------------------
	template<typename ShaderType>
	class Dx12Shader : public Dx12ShaderResource {
		std::wstring m_fileName;
	protected:
		//デリーター
		struct Deleter
		{
			void operator()(ShaderType* p) { delete p; }
		};
		ComPtr<ID3DBlob> m_shaderPtr;
		//構築と破棄
		Dx12Shader<ShaderType>(const std::wstring& fileName) : Dx12ShaderResource(), m_fileName(fileName) {}
		virtual ~Dx12Shader() {}
		static std::unique_ptr<ShaderType, Deleter> m_ptr;
	public:
		//--------------------------------------------------------------------------------------
		/*!
		@brief シェーダーのblob取得
		@return　シェーダーのblob
		*/
		//--------------------------------------------------------------------------------------
		ID3DBlob* GetShader() {
			return GetShaderBlob(m_fileName, m_shaderPtr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief シェーダーのComPtr取得
		@return　シェーダーのComPtr
		*/
		//--------------------------------------------------------------------------------------
		ComPtr<ID3DBlob>& GetShaderComPtr() {
			return GetShaderBlobComPtr(m_fileName, m_shaderPtr);
		}
		//--------------------------------------------------------------------------------------
		/*!
		@brief シェーダーのインスタンス取得
		@return　シェーダーのインスタンス
		*/
		//--------------------------------------------------------------------------------------
		static std::unique_ptr<ShaderType, Deleter>& GetPtr() {
			if (!m_ptr) {
				m_ptr.reset(new ShaderType());
			}
			return m_ptr;
		}
	};


	//これより個別シェーダー
	//シェーダ宣言用マクロ
#define DECLARE_DX12SHADER(ShaderName) class ShaderName : public Dx12Shader<ShaderName>{ \
	public: \
		ShaderName(); \
	};
	//シェーダ実体用マクロ
#define IMPLEMENT_DX12SHADER(ShaderName,CsoFilename) std::unique_ptr<ShaderName, ShaderName::Deleter> ShaderName::m_ptr; \
	ShaderName::ShaderName() : \
	Dx12Shader(CsoFilename){}

}
// end namespace basecross

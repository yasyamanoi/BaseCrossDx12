/*!
@file Light.cpp
@brief ���C�g�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	LightSet::LightSet() :
		ObjectInterface(),
		m_mainIndex(0)
	{
	}


	Light LightSet::GetLight(size_t index)const {
		if (index >= m_lights.size()) {
			throw BaseException(
				L"���C�g�̃C���f�b�N�X���͈͊O�ł�",
				Util::SizeTToWStr(index),
				L"LightSet::GetLight()"
			);
		}
		return m_lights[index];
	}
	void LightSet::SetLight(size_t index, const Light& light) {
		if (index >= m_lights.size()) {
			throw BaseException(
				L"���C�g�̃C���f�b�N�X���͈͊O�ł�",
				Util::SizeTToWStr(index),
				L"LightSet::SetLight()"
			);
		}
		m_lights[index] = light;
	}

	void LightSet::AddLight(const Light& light) {
		if (m_lights.size() >= m_maxLights) {
			throw BaseException(
				L"����ȏド�C�g�͑��₹�܂���",
				L"LightSet::AddLight()"
			);
		}
		m_lights.push_back(light);
	}

	void LightSet::OnCreate() {
		//�f�t�H���g�̃��C�g���쐬����
		const Vec3 defaultDirections[3] =
		{
			{ -0.5265408f, -0.5735765f, -0.6275069f },
			{ 0.7198464f,  0.3420201f,  0.6040227f },
			{ 0.4545195f, -0.7660444f,  0.4545195f },
		};

		const Vec4 defaultDiffuse[3] =
		{
			{ 0.3231373f, 0.3607844f, 0.3937255f,0.0f },
			{ 0.9647059f, 0.7607844f, 0.4078432f,0.0f },
			{ 1.0000000f, 0.9607844f, 0.8078432f,0.0f },
		};

		const Vec4 defaultSpecular[3] =
		{
			{ 0.3231373f, 0.3607844f, 0.3937255f,0.0f },
			{ 0.0000000f, 0.0000000f, 0.0000000f,0.0f },
			{ 1.0000000f, 0.9607844f, 0.8078432f,0.0f },
		};
		const Vec4 defaultAmbient = { 0.05333332f, 0.09882354f, 0.1819608f ,0.0f };
		m_lights.resize(3);
		for (UINT i = 0; i < 3; i++) {
			m_lights[i].m_directional = defaultDirections[i];
			m_lights[i].m_diffuseColor = defaultDiffuse[i];
			m_lights[i].m_specularColor = defaultSpecular[i];
		}
		m_ambient = defaultAmbient;
		m_mainIndex = 2;
	}



}
// end namespace basecross

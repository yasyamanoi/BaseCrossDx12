/*!
@file VertexHelper.cpp
@brief ���_�w���p�[�N���X
@copyright WiZ Tamura Hiroki,Yamanoi Yasushi MIT License (MIT).
 MIT License URL: https://opensource.org/license/mit
*/

#include "stdafx.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	// �p�r: ���_��`�֘A�̃C���v�������g
	//--------------------------------------------------------------------------------------
	void XM_CALLCONV VertexPositionNormalTangentColorTexture::SetColor( FXMVECTOR color )
	{
		DirectX::PackedVector::XMUBYTEN4 rgba;
		XMStoreUByteN4( &rgba, color );
		this->color = rgba.v;
	}

	void VertexPositionNormalTangentColorTextureSkinning::SetBlendIndices( XMUINT4 const& indices )
	{
		this->indices = ( (indices.w & 0xff) << 24 ) | ( (indices.z & 0xff) << 16 ) | ( (indices.y & 0xff) << 8 ) | ( indices.x & 0xff );
	}

	void XM_CALLCONV VertexPositionNormalTangentColorTextureSkinning::SetBlendWeights( FXMVECTOR weights )
	{
		DirectX::PackedVector::XMUBYTEN4 packed;
		XMStoreUByteN4( &packed, weights );
		this->weights = packed.v;
	}



}
//end of namespace basecross.

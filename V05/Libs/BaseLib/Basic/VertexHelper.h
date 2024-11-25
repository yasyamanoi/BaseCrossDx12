/*!
@file VertexHelper.h
@brief 頂点ヘルパークラス
@copyright Copyright (c) 2022 WiZ Tamura Hiroki,Yamanoi Yasushi.
*/

#pragma once
#include "stdafx.h"

namespace basecross {


#if (DIRECTXMATH_VERSION < 305) && !defined(XM_CALLCONV)
#define XM_CALLCONV __fastcall
typedef const XMVECTOR& HXMVECTOR;
typedef const XMMATRIX& FXMMATRIX;
#endif

	//--------------------------------------------------------------------------------------
	/// 位置を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPosition
	{
		VertexPosition()
		{ }

		VertexPosition(float x,float y,float z)
			: position(bsm::Vec3(x, y, z))
		{ }


		VertexPosition(XMFLOAT3 const& pos)
			: position(pos)
		{ }

		VertexPosition(FXMVECTOR pos)
			: position(pos)
		{ }

		bsm::Vec3 position;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionLayout );
		}
	};


	//--------------------------------------------------------------------------------------
	/// 位置と色を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionColor
	{
		VertexPositionColor()
		{ }

		VertexPositionColor(XMFLOAT3 const& pos, XMFLOAT4 const& col)
			: position(pos),
			color(col)
		{ }

		VertexPositionColor(FXMVECTOR pos, FXMVECTOR col)
			: position(pos),
			color(col)
		{
		}

		bsm::Vec3 position;
		bsm::Vec4 color;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionColorLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionColorLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置とテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionTextureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置とテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionTexture
	{
		VertexPositionTexture()
		{ }

		VertexPositionTexture(XMFLOAT3 const& pos, XMFLOAT2 const& tex)
			: position(pos),
			textureCoordinate(tex)
		{ }

		VertexPositionTexture(FXMVECTOR pos, FXMVECTOR tex)
			: position(pos),
			textureCoordinate(tex)
		{
		}

		bsm::Vec3 position;
		bsm::Vec2 textureCoordinate;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionTextureLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionTextureLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormal
	{
		VertexPositionNormal()
		{ }

		VertexPositionNormal(XMFLOAT3 const& pos, XMFLOAT3 const& norm)
			: position(pos),
			normal(norm)
		{ }

		VertexPositionNormal(FXMVECTOR pos, FXMVECTOR norm)
			: position(pos),
			normal(norm)
		{
		}

		bsm::Vec3 position;
		bsm::Vec3 normal;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionColorTextureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionColorTexture
	{
		VertexPositionColorTexture()
		{ }

		VertexPositionColorTexture(XMFLOAT3 const& pos, XMFLOAT4 const& col, XMFLOAT2 const& tex)
			: position(pos),
			color(col),
			textureCoordinate(tex)
		{ }

		VertexPositionColorTexture(FXMVECTOR pos, FXMVECTOR col, FXMVECTOR tex)
			: position(pos),
			color(col),
			textureCoordinate(tex)
		{
		}

		bsm::Vec3 position;
		bsm::Vec4 color;
		bsm::Vec2 textureCoordinate;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionColorTextureLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionColorTextureLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線と色を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線と色を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalColor
	{
		VertexPositionNormalColor()
		{ }

		VertexPositionNormalColor(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& col)
			: position(pos),
			normal(norm),
			color(col)
		{ }

		VertexPositionNormalColor(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR col)
			: position(pos),
			normal(norm),
			color(col)
		{
		}

		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec4 color;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalColorLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalColorLayout );
		}
	};



	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTextureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTexture
	{
		VertexPositionNormalTexture()
		{ }

		VertexPositionNormalTexture(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT2 const& tex)
			: position(pos),
			normal(norm),
			textureCoordinate(tex)
		{ }

		VertexPositionNormalTexture(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tex)
			: position(pos),
			normal(norm),
			textureCoordinate(tex)
		{ }

		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec2 textureCoordinate;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalTextureLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalTextureLayout );
		}
	};


	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャとスキニング情報を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTextureSkinningLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",0, DXGI_FORMAT_R32G32B32A32_UINT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};


	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャとスキニング情報を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct  VertexPositionNormalTextureSkinning
	{
		VertexPositionNormalTextureSkinning(){}
	
		VertexPositionNormalTextureSkinning(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT2 const& tex,
			uint32_t* index,float* weight)
			: position(pos),
			normal(norm),
			textureCoordinate(tex)
		{ 
			for(int i = 0;i < 4;i++){
				indices[i] = index[i];
				weights[i] = weight[i];
			}
		}

		VertexPositionNormalTextureSkinning(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tex,
			uint32_t* index, float* weight)
			: position(pos),
			normal(norm),
			textureCoordinate(tex)
		{ 
			for (int i = 0; i < 4; i++){
				indices[i] = index[i];
				weights[i] = weight[i];
			}
		}


		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec2 textureCoordinate;
		uint32_t indices[4];
		float weights[4];

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalTextureSkinningLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalTextureSkinningLayout );
		}
	};



	//--------------------------------------------------------------------------------------
	/// 位置と法線と色とテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalColorTextureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線と色とテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalColorTexture
	{
		VertexPositionNormalColorTexture()
		{ }

		VertexPositionNormalColorTexture(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& col, XMFLOAT2 const& tex)
			: position(pos),
			normal(norm),
			color(col),
			textureCoordinate(tex)
		{ }

		VertexPositionNormalColorTexture(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR col, CXMVECTOR tex)
			: position(pos),
			normal(norm),
			color(col),
			textureCoordinate(tex)
		{ }

		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec4 color;
		bsm::Vec2 textureCoordinate;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalColorTextureLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalColorTextureLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントとテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTangentTextureLayout[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントとテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTangentTexture
	{
		VertexPositionNormalTangentTexture()
		{ }


		VertexPositionNormalTangentTexture(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& tan,  XMFLOAT2 const& tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
		}

		VertexPositionNormalTangentTexture(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tan,  CXMVECTOR tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
		}


		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec4 tangent;
		bsm::Vec2 textureCoordinate;


		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionNormalTangentTextureLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionNormalTangentTextureLayout);
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャとタンジェントとを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTextureTangentLayout[] =
	{

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントとテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTextureTangent
	{
		VertexPositionNormalTextureTangent()
		{ }


		VertexPositionNormalTextureTangent(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& tan, XMFLOAT2 const& tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
		}

		VertexPositionNormalTextureTangent(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tan, CXMVECTOR tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
		}


		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec2 textureCoordinate;
		bsm::Vec4 tangent;


		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionNormalTextureTangentLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionNormalTextureTangentLayout);
		}
	};


	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントとテクスチャとスキン情報を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTangentTextureSkinningLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",0, DXGI_FORMAT_R32G32B32A32_UINT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントとテクスチャとスキン情報を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTangentTextureSkinning
	{
		VertexPositionNormalTangentTextureSkinning()
		{ }


		VertexPositionNormalTangentTextureSkinning(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& tan, XMFLOAT2 const& tex,
			uint32_t* index, float* weight)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
			for (int i = 0; i < 4; i++) {
				indices[i] = index[i];
				weights[i] = weight[i];
			}
		}

		VertexPositionNormalTangentTextureSkinning(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tan, CXMVECTOR tex,
			uint32_t* index, float* weight)
			: position(pos),
			normal(norm),
			tangent(tan),
			textureCoordinate(tex)
		{
			for (int i = 0; i < 4; i++) {
				indices[i] = index[i];
				weights[i] = weight[i];
			}
		}


		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec4 tangent;
		bsm::Vec2 textureCoordinate;
		uint32_t indices[4];
		float weights[4];


		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionNormalTangentTextureSkinningLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionNormalTangentTextureSkinningLayout);
		}
	};




	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントと色とテクスチャを持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTangentColorTextureLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントと色とテクスチャを持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTangentColorTexture
	{
		VertexPositionNormalTangentColorTexture()
		{ }


		VertexPositionNormalTangentColorTexture(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT4 const& tan, uint32_t rgba, XMFLOAT2 const& tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			color(rgba),
			textureCoordinate(tex)
		{
		}

		VertexPositionNormalTangentColorTexture(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tan, uint32_t rgba, CXMVECTOR tex)
			: position(pos),
			normal(norm),
			tangent(tan),
			color(rgba),
			textureCoordinate(tex)
		{
		}

		VertexPositionNormalTangentColorTexture(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT4 const& tangent, XMFLOAT4 const& color, XMFLOAT2 const& textureCoordinate)
			: position(position),
			normal(normal),
			tangent(tangent),
			textureCoordinate(textureCoordinate)
		{
			SetColor( color );
		}

		VertexPositionNormalTangentColorTexture(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, CXMVECTOR color, CXMVECTOR textureCoordinate)
		{
			XMStoreFloat3(&this->position, position);
			XMStoreFloat3(&this->normal, normal);
			XMStoreFloat4(&this->tangent, tangent);
			XMStoreFloat2(&this->textureCoordinate, textureCoordinate);

			SetColor( color );
		}

		void SetColor( XMFLOAT4 const& color ) { SetColor( XMLoadFloat4( &color ) ); }
		void XM_CALLCONV SetColor( FXMVECTOR color );

		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec4 tangent;
		uint32_t color;
		bsm::Vec2 textureCoordinate;


		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalTangentColorTextureLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalTangentColorTextureLayout );
		}
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントと色とテクスチャとスキニング情報を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTangentColorTextureSkinningLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",0, DXGI_FORMAT_R8G8B8A8_UINT,      0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とタンジェントと色とテクスチャとスキニング情報を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct VertexPositionNormalTangentColorTextureSkinning : public VertexPositionNormalTangentColorTexture
	{
		VertexPositionNormalTangentColorTextureSkinning()
		{ }

		uint32_t indices;
		uint32_t weights;

		VertexPositionNormalTangentColorTextureSkinning(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT4 const& tangent, uint32_t rgba,
									XMFLOAT2 const& textureCoordinate, XMUINT4 const& indices, XMFLOAT4 const& weights)
			: VertexPositionNormalTangentColorTexture(position,normal,tangent,rgba,textureCoordinate)
		{
			SetBlendIndices( indices );
			SetBlendWeights( weights );
		}

		VertexPositionNormalTangentColorTextureSkinning(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, uint32_t rgba, CXMVECTOR textureCoordinate,
									XMUINT4 const& indices, CXMVECTOR weights)
			: VertexPositionNormalTangentColorTexture(position,normal,tangent,rgba,textureCoordinate)
		{
			SetBlendIndices( indices );
			SetBlendWeights( weights );
		}

		VertexPositionNormalTangentColorTextureSkinning(XMFLOAT3 const& position, XMFLOAT3 const& normal, XMFLOAT4 const& tangent, XMFLOAT4 const& color,
									XMFLOAT2 const& textureCoordinate, XMUINT4 const& indices, XMFLOAT4 const& weights)
			: VertexPositionNormalTangentColorTexture(position,normal,tangent,color,textureCoordinate)
		{
			SetBlendIndices( indices );
			SetBlendWeights( weights );
		}

		VertexPositionNormalTangentColorTextureSkinning(FXMVECTOR position, FXMVECTOR normal, FXMVECTOR tangent, CXMVECTOR color, CXMVECTOR textureCoordinate,
									XMUINT4 const& indices, CXMVECTOR weights)
			: VertexPositionNormalTangentColorTexture(position,normal,tangent,color,textureCoordinate)
		{
			SetBlendIndices( indices );
			SetBlendWeights( weights );
		}

		void SetBlendIndices( XMUINT4 const& indices );

		void SetBlendWeights( XMFLOAT4 const& weights ) { SetBlendWeights( XMLoadFloat4( &weights ) ); }
		void XM_CALLCONV SetBlendWeights( FXMVECTOR weights );

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement(){
			return VertexPositionNormalTangentColorTextureSkinningLayout;
		}
		static UINT GetNumElements(){
			return  ARRAYSIZE( VertexPositionNormalTangentColorTextureSkinningLayout );
		}

	};


	//--------------------------------------------------------------------------------------
	/// 位置と色と行列を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionColorMatrixLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャと行列を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct  VertexPositionColorMatrix
	{
		VertexPositionColorMatrix()
		{ }


		VertexPositionColorMatrix(XMFLOAT3 const& pos, XMFLOAT4 const& col, 
			XMFLOAT4X4 const& mat)
			: position(pos),
			color(col),
			matrix(mat)
		{ }



		VertexPositionColorMatrix(FXMVECTOR pos, FXMVECTOR col, 
			FXMMATRIX const& mat)
			: position(pos),
			color(col),
			matrix(mat)
		{
		}

		bsm::Vec3 position;
		bsm::Vec4 color;
		bsm::Mat4x4 matrix;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionColorMatrixLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionColorMatrixLayout);
		}
	};



	//--------------------------------------------------------------------------------------
	/// 位置とテクスチャと行列を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionTextureMatrixLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャと行列を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct  VertexPositionTextureMatrix
	{
		VertexPositionTextureMatrix()
		{ }


		VertexPositionTextureMatrix(XMFLOAT3 const& pos,  XMFLOAT2 const& tex,
			XMFLOAT4X4 const& mat)
			: position(pos),
			textureCoordinate(tex),
			matrix(mat)
		{ }



		VertexPositionTextureMatrix(FXMVECTOR pos,  FXMVECTOR tex,
			FXMMATRIX const& mat)
			: position(pos),
			textureCoordinate(tex),
			matrix(mat)
		{
		}

		bsm::Vec3 position;
		bsm::Vec2 textureCoordinate;
		bsm::Mat4x4 matrix;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionTextureMatrixLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionTextureMatrixLayout);
		}
	};



	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャと行列を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionColorTextureMatrixLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と色とテクスチャと行列を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct  VertexPositionColorTextureMatrix
	{
		VertexPositionColorTextureMatrix()
		{ }


		VertexPositionColorTextureMatrix(XMFLOAT3 const& pos, XMFLOAT4 const& col, XMFLOAT2 const& tex,
			XMFLOAT4X4 const& mat)
			: position(pos),
			color(col),
			textureCoordinate(tex),
			matrix(mat)
		{ }



		VertexPositionColorTextureMatrix(FXMVECTOR pos, FXMVECTOR col, FXMVECTOR tex,
			FXMMATRIX const& mat)
			: position(pos),
			color(col),
			textureCoordinate(tex),
			matrix(mat)
		{
		}

		bsm::Vec3 position;
		bsm::Vec4 color;
		bsm::Vec2 textureCoordinate;
		bsm::Mat4x4 matrix;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionColorTextureMatrixLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionColorTextureMatrixLayout);
		}
	};


	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャと行列を持つ入力レイアウトの定義
	//--------------------------------------------------------------------------------------
	const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTextureMatrixLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,  0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,     0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "MATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
		{ "MATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	};

	//--------------------------------------------------------------------------------------
	/// 位置と法線とテクスチャと行列を持つ頂点の定義
	//--------------------------------------------------------------------------------------
	struct  VertexPositionNormalTextureMatrix
	{
		VertexPositionNormalTextureMatrix()
		{ }

		VertexPositionNormalTextureMatrix(XMFLOAT3 const& pos, XMFLOAT3 const& norm, XMFLOAT2 const& tex,
			XMFLOAT4X4 const& mat)
			: position(pos),
			normal(norm),
			textureCoordinate(tex),
			matrix(mat)
		{ }

		VertexPositionNormalTextureMatrix(FXMVECTOR pos, FXMVECTOR norm, FXMVECTOR tex,
			FXMMATRIX const& mat)
			: position(pos),
			normal(norm),
			textureCoordinate(tex),
			matrix(mat)
		{
		}

		bsm::Vec3 position;
		bsm::Vec3 normal;
		bsm::Vec2 textureCoordinate;
		bsm::Mat4x4 matrix;

		static const D3D12_INPUT_ELEMENT_DESC* GetVertexElement() {
			return VertexPositionNormalTextureMatrixLayout;
		}
		static UINT GetNumElements() {
			return  ARRAYSIZE(VertexPositionNormalTextureMatrixLayout);
		}
	};


}
//end basecross

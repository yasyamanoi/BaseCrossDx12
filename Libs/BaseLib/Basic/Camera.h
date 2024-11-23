
#pragma once
#include "stdafx.h"


namespace basecross {

	class Camera
	{
	public:
		Camera();
		~Camera();


		void Get3DViewProjMatricesLH(XMFLOAT4X4* view, XMFLOAT4X4* proj, float fovInDegrees, float screenWidth, float screenHeight);
		void Get3DViewProjMatrices(XMFLOAT4X4* view, XMFLOAT4X4* proj, float fovInDegrees, float screenWidth, float screenHeight);
		void Reset();
		void Set(XMVECTOR eye, XMVECTOR at, XMVECTOR up);
		static Camera* get();
		void RotateAroundYAxis(float angleRad);
		void RotateYaw(float angleRad);
		void RotatePitch(float angleRad);
		void GetOrthoProjMatrices(XMFLOAT4X4* view, XMFLOAT4X4* proj, float width, float height);
		XMVECTOR mEye; // Where the camera is in world space. Z increases into of the screen when using LH coord system (which we are and DX uses)
		XMVECTOR mAt; // What the camera is looking at (world origin)
		XMVECTOR mUp; // Which way is up
	private:
		static Camera* mCamera;
	};
}
using namespace basecross;
//end namespace basecross

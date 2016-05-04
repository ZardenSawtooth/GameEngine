#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H

#include "../Math/cQuaternion.h"
#include "../Math/cVector.h"
#include "../Math/cMatrix_transformation.h"

class Camera
{
public:

	eae6320::Math::cQuaternion m_Orientation;
	eae6320::Math::cVector m_Position = eae6320::Math::cVector(0,0,0);
	eae6320::Math::cVector m_PositionPlayer = eae6320::Math::cVector(0, 0, 0);
	eae6320::Math::cVector m_PositionPlayerRay = eae6320::Math::cVector(0, 0, 0);

	float eulerX, eulerY, eulerZ;
	float AspectRatio;

	static Camera& getInstance()
	{
		static Camera    instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}
private:
	Camera() { eulerX = eulerY = eulerZ = 0; };                   // Constructor? (the {} brackets) are needed here.
	eae6320::Math::cVector getLocalX()
	{
		return eae6320::Math::cVector(1.0f, 0.0f, 0.0f) * eae6320::Math::cMatrix_transformation(m_Orientation, eae6320::Math::cVector(0.0f, 0.0f, 0.0f));
	}
	eae6320::Math::cVector getLocalY()
	{
		return eae6320::Math::cVector(0.0f, 1.0f, 0.0f) * eae6320::Math::cMatrix_transformation(m_Orientation, eae6320::Math::cVector(0.0f, 0.0f, 0.0f));
	}
	eae6320::Math::cVector getLocalZ()
	{
		return eae6320::Math::cVector(0.0f, 0.0f, 1.0f) * eae6320::Math::cMatrix_transformation(m_Orientation, eae6320::Math::cVector(0.0f, 0.0f, 0.0f));
	}
							  // C++ 11
							  // =======
							  // We can use the better technique of deleting the methods
							  // we don't want.
	Camera(Camera const&) = delete;
	void operator=(Camera const&) = delete;

};


#endif
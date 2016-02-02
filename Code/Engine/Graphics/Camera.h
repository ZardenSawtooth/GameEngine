#pragma once
#ifndef _CAMERA_H
#define _CAMERA_H

#include "../Math/cQuaternion.h"
#include "../Math/cVector.h"

class Camera
{
public:

	eae6320::Math::cQuaternion m_Orientation;
	eae6320::Math::cVector m_Position = eae6320::Math::cVector(0,0,0);

	static Camera& getInstance()
	{
		static Camera    instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
		return instance;
	}
private:
	Camera() {};                   // Constructor? (the {} brackets) are needed here.

							  // C++ 11
							  // =======
							  // We can use the better technique of deleting the methods
							  // we don't want.
	Camera(Camera const&) = delete;
	void operator=(Camera const&) = delete;

};


#endif
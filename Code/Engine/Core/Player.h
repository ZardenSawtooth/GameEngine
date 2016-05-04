#pragma once

#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "TPCamera.h"

namespace eae6320
{
	namespace Core
	{
		class Player
		{
		public:
			Math::cVector Position;
			Math::cQuaternion Orientation;
			Math::cVector Velocity;
			float eulerX, eulerY, eulerZ;
			void Update(float dt);

			void UpdateInput();
			void UpdateCamera(TPCamera * camera);

			Math::cVector getLocalX();
			Math::cVector getLocalY();
			Math::cVector getLocalZ();

		private:
			float height = 50;
			
		};
	}
}
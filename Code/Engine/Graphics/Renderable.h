#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include "Mesh.h"
#include "Effect.h"
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include <vector>

namespace eae6320 
{

	namespace Graphics 
	{
		struct Renderable
		{
			struct PositionOffset
			{
				float x = 0.0f;
				float y = 0.0f;
			};
			Mesh mMesh;
			Effect mEffect;
			Math::cVector m_position;
			Math::cQuaternion m_orientation;
			PositionOffset mPositionOffset;
		}; 

		extern std::vector <Renderable*>  RenderableList;

	}
}


#endif // !_RENDERABLE_H

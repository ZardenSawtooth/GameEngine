#ifndef _RENDERABLE_H
#define _RENDERABLE_H

#include "Mesh.h"
#include "Material.h"
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include <vector>
#include "GameSprite.h"

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
			//Effect mEffect;
			Material m_Material;

			Math::cVector m_position;
			Math::cQuaternion m_orientation;
			PositionOffset mPositionOffset;
		}; 

		extern std::vector <Renderable*>  RenderableList;
		extern std::vector <GameSprite*>  GameSpriteList;

	}
}


#endif // !_RENDERABLE_H

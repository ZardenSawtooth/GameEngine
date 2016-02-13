#include "Graphics.h"
#include "Renderable.h"
#include "Effect.h"
#include "Material.h"
#include "GameSprite.h"

namespace eae6320
{
	void eae6320::Graphics::Render(float gameTime)
	{
		// Every frame an entirely new image will be created.
		// Before drawing anything, then, the previous image will be erased
		// by "clearing" the image buffer (filling it with a solid color)
		Clear();
		// The actual function calls that draw geometry must be made between paired calls to
		// BeginScene() and EndScene()
		{
			BeginScene();
			{	
				GameSpriteList[0]->Draw(gameTime);
				
				GameSpriteList[1]->Draw(gameTime);

				for (unsigned int i = 0; i < RenderableList.size(); i++) 
				{
					//SetEffect(RenderableList[i]->mEffect);
					SetMaterial(RenderableList[i]->m_Material);

					Math::cMatrix_transformation matrix(RenderableList[i]->m_orientation, RenderableList[i]->m_position);
					eae6320::Graphics::SetDrawCallUniforms(RenderableList[i]->m_Material.m_effect, matrix);

					



					eae6320::Graphics::DrawMesh(RenderableList[i]->mMesh);
				}
#ifdef _DEBUG
					eae6320::Graphics::RenderDebugShapes();
#endif		
				
				
			}
			EndScene();
		}
	
		// Everything has been drawn to the "back buffer", which is just an image in memory.
		// In order to display it, the contents of the back buffer must be "presented"
		// (to the front buffer)
		DisplayRenderedBuffer();
	}

}
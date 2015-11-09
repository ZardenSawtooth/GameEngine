#include "Graphics.h"
#include "Renderable.h"
#include "Effect.h"

namespace eae6320 
{
	void eae6320::Graphics::Render()
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
				for (int i = 0; i < 1/*RenderableList.size()*/; i++) {
					SetEffect(RenderableList[i]->mEffect);

					Math::cMatrix_transformation matrix(RenderableList[0]->m_orientation, RenderableList[0]->m_position) ;

					eae6320::Graphics::SetDrawCallUniforms(RenderableList[i]->mEffect, matrix);
					eae6320::Graphics::DrawMesh(RenderableList[i]->mMesh);
				}
				
			}
			EndScene();
			
			
	
		}
	
		// Everything has been drawn to the "back buffer", which is just an image in memory.
		// In order to display it, the contents of the back buffer must be "presented"
		// (to the front buffer)
		DisplayRenderedBuffer();
	}

}
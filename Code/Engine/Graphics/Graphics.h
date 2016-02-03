/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "../Windows/Includes.h"

#include <d3d9.h>


// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		bool Initialize( const HWND i_renderingWindow );
		void Render();
		bool ShutDown();

		bool Clear();
		bool BeginScene();
		bool EndScene();
		bool DisplayRenderedBuffer();

		IDirect3DDevice9* getDirect3DDevice();

		

		void RenderDebugShapes();
		
	}
}

#endif	// EAE6320_GRAPHICS_H

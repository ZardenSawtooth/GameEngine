#pragma once

// This file contains the function declarations for the Mesh

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Header Files
//=============

#include "../Windows/Includes.h"
#include <d3d9.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../UserOutput/UserOutput.h"
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"


// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{

		struct  Effect
		{

#if defined( EAE6320_PLATFORM_D3D )

			IDirect3DVertexShader9* s_vertexShader = NULL;
			IDirect3DPixelShader9* s_fragmentShader = NULL;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_programId = 0;
#endif
		};

#if defined( EAE6320_PLATFORM_D3D )
		bool LoadFragmentShader(Effect &i_Effect, const char * i_path);
		bool LoadVertexShader(Effect &i_Effect, const char * i_path);
#elif defined( EAE6320_PLATFORM_GL )
		bool LoadFragmentShader(Effect &i_Effect, const char * i_path);
		bool LoadVertexShader(Effect &i_Effect, const char * i_path);

		struct sLogInfo
		{
			GLchar* memory;
			sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
			~sLogInfo() { if (memory) free(memory); }
		};
		
#endif

		void SetEffect(const Effect &i_Effect);
		bool LoadEffect(Effect &i_Effect, const char * i_FragmentPath, const char * i_VertexPath);

	}
}

#endif 
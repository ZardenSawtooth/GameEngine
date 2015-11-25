#pragma once

// This file contains the function declarations for the Mesh

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Header Files
//=============

#include "../Windows/Includes.h"
#include "../UserOutput/UserOutput.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"
#include "../Graphics/Camera.h"

#if defined( EAE6320_PLATFORM_D3D )

#include <d3dx9shader.h>
#include <d3d9.h>
#elif defined( EAE6320_PLATFORM_GL )
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif




// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		typedef
#if defined( EAE6320_PLATFORM_D3D )
			// This is conceptually a D3DXHANDLE but is defined this way
			// so that external files aren't required to specify the #include path to the DirectX SDK
			const char*
#elif defined( EAE6320_PLATFORM_GL )
			// This is conceptually a GLint but is defined this way
			// so that external files aren't required to specify the #include path to the OpenGl SDK
			GLint
#endif
			tUniformHandle;
		enum  eShaderType
		{
			fragment, vertex
		};


		struct  Effect
		{



			uint8_t renderstates = 0;

#if defined( EAE6320_PLATFORM_D3D )

			IDirect3DVertexShader9* s_vertexShader = NULL;
			IDirect3DPixelShader9* s_fragmentShader = NULL;
			D3DXHANDLE handle_localToWorld = NULL;
			D3DXHANDLE handle_worldToView = NULL;
			D3DXHANDLE handle_viewToScreen = NULL;
			ID3DXConstantTable* vertexShaderConstantTable = NULL;
			ID3DXConstantTable* fragmentShaderConstantTable = NULL;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_programId = 0;
			GLint location_localToWorld = -1;
			GLint location_worldToView = -1;
			GLint location_viewToScreen = -1;
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
		bool LoadEffect(Effect &i_Effect, const char * i_path);

		tUniformHandle SetMaterialUniforms(const char * i_uniformName, Effect &i_Effect);

		void SetUniforms(Effect &i_Effect, Graphics::tUniformHandle i_UniformHandle, Graphics::eShaderType shaderType, float i_values[], uint8_t i_valueCounttoSet);

		bool CreateProgram(eae6320::Graphics::Effect &i_Effect);
		bool LinkProgram(eae6320::Graphics::Effect &i_Effect);
		bool SetDrawCallUniforms(const Effect &i_Effect, Math::cMatrix_transformation i_localToWorldTransform);

	}
}

#endif 
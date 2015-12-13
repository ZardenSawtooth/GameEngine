#pragma once

#ifndef _MATERIALBUILDER_H
#define _MATERIALBUILDER_H

#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"
#include "Effect.h"


namespace eae6320 
{
	namespace Graphics
	{
		struct sUniformData
		{
			tUniformHandle uniformHandle = NULL;
			eShaderType shaderType;
			float values[4];
			uint8_t valueCountToSet;
		};

		struct Material
		{
			Effect m_effect;
			sUniformData * uniformArray;
			uint8_t m_numberOfUniforms;
			

#if defined( EAE6320_PLATFORM_D3D )
			IDirect3DTexture9 * m_texture;
			DWORD samplerID;

#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_texture;
			GLint samplerID;
#endif


		};


		void LoadMaterial(Material &i_Material, const char * i_path);
		bool LoadTexture(Material &i_Material, const char * i_path, const char * i_textureHandle);

		void SetMaterial(Material &i_Material);
		void SetTextures(Material &i_Material);
	}

}



#endif //_MATERIALBUILDER_H
#pragma once
/*
This is an example builder program that just copies the source file to the target path
(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_MATERIALBUILDER_H
#define EAE6320_MATERIALBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================
enum  eShaderType
{
	 fragment, vertex
};

typedef
#if defined( EAE6320_PLATFORM_D3D )
// This is conceptually a D3DXHANDLE but is defined this way
// so that external files aren't required to specify the #include path to the DirectX SDK
	const char*
#elif defined( EAE6320_PLATFORM_GL )
// This is conceptually a GLint but is defined this way
// so that external files aren't required to specify the #include path to the OpenGl SDK
	int32_t
#endif
    tUniformHandle;



namespace eae6320
{
	struct sParameterToSet
	{
		/*cEffect::*/tUniformHandle uniformHandle = NULL;
		/*ShaderTypes::*/eShaderType shaderType;
		float values[4];
		uint8_t valueCountToSet;
	};

	class MaterialBuilder : public cbBuilder
	{
		// Interface
		//==========



	public:

		// Build
		//------



		virtual bool Build(const std::vector<std::string>& i_arguments);
		bool LoadMaterialData(const char * i_path);




	};
}



#endif	// EAE6320_MATERIALBUILDER_H

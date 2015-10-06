/*
This is an example builder program that just copies the source file to the target path
(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CGENERICBUILDER_H
#define EAE6320_CGENERICBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{
	struct sVertex
	{
		// POSITION
		// 2 floats == 8 bytes
		// Offset = 0
		float x, y;
		// COLOR0
		// 4 uint8_ts == 4 bytes
		// Offset = 8
		uint8_t b, g, r, a;	// Direct3D expects the byte layout of a color to be different from what you might expect
	};

	class cMeshBuilder : public cbBuilder
	{
		// Interface
		//==========
		
		

	public:

		// Build
		//------

		 

		virtual bool Build(const std::vector<std::string>& i_arguments);
		bool LoadMeshData(const char * i_path);

		
		 
		
	};
}



#endif	// EAE6320_CGENERICBUILDER_H

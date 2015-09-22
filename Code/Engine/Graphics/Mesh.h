#pragma once

// This file contains the function declarations for the Mesh

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

// Header Files
//=============

#include "../Windows/Includes.h"
#include <d3d9.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../UserOutput/UserOutput.h"


// Interface
//==========

namespace eae6320
{
	namespace Graphics
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

		struct  Mesh
		{

			#if defined( EAE6320_PLATFORM_D3D )
				IDirect3DVertexDeclaration9* s_vertexDeclaration = NULL;
				IDirect3DIndexBuffer9* s_indexBuffer = NULL;
				IDirect3DVertexBuffer9* s_vertexBuffer = NULL;


			#elif defined( EAE6320_PLATFORM_GL )
				GLuint s_vertexArrayId = 0;
			#endif

				
		};

		#if defined( EAE6320_PLATFORM_D3D )
			HRESULT GetVertexProcessingUsage(DWORD& o_usage);
		#elif defined( EAE6320_PLATFORM_GL )
			bool CreateVertexArray(Mesh &i_Mesh);
		#endif

		bool CreateVertexBuffer( sVertex* i_vertexData, Mesh &i_Mesh);
		bool CreateIndexBuffer( uint32_t * i_indexData,  Mesh &i_Mesh);

		
		void DrawMesh(const Mesh &i_Mesh );

		bool LoadMesh( Mesh &i_Mesh);
		
	}
}

#endif 
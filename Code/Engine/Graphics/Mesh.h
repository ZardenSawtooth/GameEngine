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


// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		
		//IDirect3DDevice9* s_direct3dDevice = NULL;

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

		//bool DrawMesh(const Mesh &i_Mesh, const unsigned int i_sizeOfsVertex );
		void DrawMesh(const Mesh &i_Mesh, const unsigned int i_sizeOfsVertex );
	}
}

#endif 
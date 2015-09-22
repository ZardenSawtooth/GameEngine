// Header Files
//=============

#include "Mesh.h"
#include "Graphics.h"
#include <cassert>
#include <cstdint>
#include <d3d9.h>
#include <d3dx9shader.h>
#include <sstream>

namespace eae6320
{
	void Graphics::DrawMesh(const Mesh &i_Mesh) {

		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		// Bind a specific vertex buffer to the device as a data source
		{
			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(sVertex);
			HRESULT result = s_direct3dDevice->SetStreamSource(streamIndex, i_Mesh.s_vertexBuffer, bufferOffset, bufferStride);
			assert(SUCCEEDED(result));
		}
		// Bind a specific index buffer to the device as a data source
		{
			HRESULT result = s_direct3dDevice->SetIndices(i_Mesh.s_indexBuffer);
			assert(SUCCEEDED(result));
		}
		// Render objects from the current streams
		{
			// We are using triangles as the "primitive" type,
			// and we have defined the vertex buffer as a triangle list
			// (meaning that every triangle is defined by three vertices)
			const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
			// It's possible to start rendering primitives in the middle of the stream
			const unsigned int indexOfFirstVertexToRender = 0;
			const unsigned int indexOfFirstIndexToUse = 0;
			// We are drawing a square
			const unsigned int vertexCountToRender = 4;	// How vertices from the vertex buffer will be used?
			const unsigned int primitiveCountToRender = 2;	// How many triangles will be drawn?
			HRESULT result = s_direct3dDevice->DrawIndexedPrimitive(primitiveType,
				indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender,
				indexOfFirstIndexToUse, primitiveCountToRender);
			assert(SUCCEEDED(result));
		}
	}

	
	HRESULT Graphics:: GetVertexProcessingUsage(DWORD& o_usage)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = s_direct3dDevice->GetCreationParameters(&deviceCreationParameters);
		if (SUCCEEDED(result))
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
			o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			eae6320::UserOutput::Print("Direct3D failed to get the device's creation parameters");
		}
		return result;
	}

	bool Graphics::CreateVertexBuffer( sVertex* i_vertexData,  Mesh &i_Mesh)
	{
		
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				return false;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		// Initialize the vertex format
		{
			// These elements must match the VertexFormat::sVertex layout struct exactly.
			// They instruct Direct3D how to match the binary data in the vertex buffer
			// to the input elements in a vertex shader
			// (by using D3DDECLUSAGE enums here and semantics in the shader,
			// so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
			// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
			D3DVERTEXELEMENT9 vertexElements[] =
			{
				// Stream 0

				// POSITION
				// 2 floats == 8 bytes
				// Offset = 0
				{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

				// COLOR0
				// D3DCOLOR == 4 bytes
				// Offset = 8
				{ 0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

				// The following marker signals the end of the vertex declaration
				D3DDECL_END()
			};
			HRESULT result = s_direct3dDevice->CreateVertexDeclaration(vertexElements, &i_Mesh.s_vertexDeclaration);
			if (SUCCEEDED(result))
			{
				result = s_direct3dDevice->SetVertexDeclaration(i_Mesh.s_vertexDeclaration);
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to set the vertex declaration");
					return false;
				}
			}
			else
			{
				eae6320::UserOutput::Print("Direct3D failed to create a Direct3D9 vertex declaration");
				return false;
			}
		}

		// Create a vertex buffer
		{
			// We are drawing one square
			const unsigned int vertexCount = 4;	// What is the minimum number of vertices a square needs (so that no data is duplicated)?
			const unsigned int bufferSize = vertexCount * sizeof(sVertex);
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;
			
			const HRESULT result = s_direct3dDevice->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
				&i_Mesh.s_vertexBuffer, notUsed);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create a vertex buffer");
				return false;
			}
		}
		// Fill the vertex buffer with the triangle's vertices
		{
			// Before the vertex buffer can be changed it must be "locked"
			sVertex* vertexData;
			
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = i_Mesh.s_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to lock the vertex buffer");
					return false;
				}
			}

			memcpy(vertexData, i_vertexData, sizeof(i_vertexData[0]) * 4);
			// Fill the buffer
			{
				// You will need to fill in two pieces of information for each vertex:
				//	* 2 floats for the POSITION
				//	* 4 uint8_ts for the COLOR

				// The floats for POSITION are for t0he X and Y coordinates, like in Assignment 02.
				// The difference this time is that there should be fewer (because we are sharing data).

				// The uint8_ts for COLOR are "RGBA", where "RGB" stands for "Red Green Blue" and "A" for "Alpha".
				// Conceptually each of these values is a [0,1] value, but we store them as an 8-bit value to save space
				// (color doesn't need as much precision as position),
				// which means that the data we send to the GPU will be [0,255].
				// For now the alpha value should _always_ be 255, and so you will choose color by changing the first three RGB values.
				// To make white you should use (255, 255, 255), to make black (0, 0, 0).
				// To make pure red you would use the max for R and nothing for G and B, so (1, 0, 0).
				// Experiment with other values to see what happens!

				/*vertexData[0].x = 0.0f;
				vertexData[0].y = 0.0f;
				// Red
				vertexData[0].r = 255;
				vertexData[0].g = 0;
				vertexData[0].b = 0;
				vertexData[0].a = 255;

				vertexData[1].x = 0.0f;
				vertexData[1].y = 1.0f;
				// Blue
				vertexData[1].r = 0;
				vertexData[1].g = 0;
				vertexData[1].b = 255;
				vertexData[1].a = 255;

				vertexData[2].x = 1.0f;
				vertexData[2].y = 1.0f;
				// Green
				vertexData[2].r = 0;
				vertexData[2].g = 255;
				vertexData[2].b = 0;
				vertexData[2].a = 255;

				vertexData[3].x = 1.0f;
				vertexData[3].y = 0.0f;
				// Red
				vertexData[3].r = 255;
				vertexData[3].g = 0;
				vertexData[3].b = 0;
				vertexData[3].a = 255;*/

				//vertexData[1].x = EAE6320;
				// etc.
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = i_Mesh.s_vertexBuffer->Unlock();
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to unlock the vertex buffer");
					return false;
				}
			}
		}

		return true;
	}

	bool Graphics::CreateIndexBuffer(uint32_t * i_indexData,  Mesh &i_Mesh)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				return false;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		// Create an index buffer
		unsigned int bufferSize;
		{
			// We are drawing a square
			const unsigned int triangleCount = 2;	// How many triangles does a square have?
			const unsigned int vertexCountPerTriangle = 3;
			bufferSize = triangleCount * vertexCountPerTriangle * sizeof(uint32_t);
			// We'll use 32-bit indices in this class to keep things simple
			// (i.e. every index will be a 32 bit unsigned integer)
			const D3DFORMAT format = D3DFMT_INDEX32;
			// Place the index buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;
			const HRESULT result = s_direct3dDevice->CreateIndexBuffer(bufferSize, usage, format, useDefaultPool,
				&i_Mesh.s_indexBuffer, notUsed);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create an index buffer");
				return false;
			}
		}
		// Fill the index buffer with the triangles' connectivity data
		{
			// Before the index buffer can be changed it must be "locked"
			uint32_t* indexData;
			
			{
				const unsigned int lockEntireBuffer = 0;
				const DWORD useDefaultLockingBehavior = 0;
				const HRESULT result = i_Mesh.s_indexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>(&indexData), useDefaultLockingBehavior);
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to lock the index buffer");
					return false;
				}
			}
			memcpy(indexData, i_indexData, sizeof(uint32_t) * 6);
			// Fill the buffer
			{
				// EAE6320_TODO:
				// You will need to fill in 3 indices for each triangle that needs to be drawn.
				// Each index will be a 32-bit unsigned integer,
				// and will index into the vertex buffer array that you have created.
				// The order of indices is important, but the correct order will depend on
				// which vertex you have assigned to which spot in your vertex buffer
				// (also remember to maintain the correct handedness for the triangle winding order).

				// Triangle 0
				//indexData[0] = 0;
				//indexData[1] = 1;
				//indexData[2] = 2;

				//// Triangle 1
				//indexData[3] = 0;
				//indexData[4] = 2;
				//indexData[5] = 3;

				// etc...
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = i_Mesh.s_indexBuffer->Unlock();
				if (FAILED(result))
				{
					eae6320::UserOutput::Print("Direct3D failed to unlock the index buffer");
					return false;
				}
			}
		}

		return true;
	}
}
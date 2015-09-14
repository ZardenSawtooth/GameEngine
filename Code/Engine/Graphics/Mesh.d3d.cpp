// Header Files
//=============

#include "Mesh.h"
#include <cassert>
#include <cstdint>
#include <d3d9.h>
#include <d3dx9shader.h>
#include <sstream>


namespace eae6320
{
	void Graphics::DrawMesh(const Mesh &i_Mesh, const unsigned int i_sizeOfsVertex) {

		//extern IDirect3DDevice9* s_direct3dDevice;
		// Bind a specific vertex buffer to the device as a data source
		{
			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = i_sizeOfsVertex;
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
}
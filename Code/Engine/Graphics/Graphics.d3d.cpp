// Header Files
//=============

#include "Graphics.h"
#include "Mesh.h"
#include "Effect.h"
#include <cassert>
#include <cstdint>
#include <d3d9.h>
#include <d3dx9shader.h>
#include <sstream>
#include "Renderable.h"
#include "../Math/cVector.h"


// Static Data Initialization
//===========================

//IDirect3DDevice9* s_direct3dDevice = NULL;
 std::vector <eae6320::Graphics::Renderable*>  eae6320::Graphics::RenderableList;
eae6320::Graphics::Renderable renderableTriangle1;
eae6320::Graphics::Renderable renderableTriangle2;
eae6320::Graphics::Renderable renderableSquare;
eae6320::Graphics::Renderable renderableFloor;
eae6320::Graphics::Renderable renderableObject;
eae6320::Graphics::Renderable renderableSphere;

namespace
{
	IDirect3DDevice9* s_direct3dDevice = NULL;
	HWND s_renderingWindow = NULL;
	IDirect3D9* s_direct3dInterface = NULL;
	
	
	// This struct determines the layout of the data that the CPU will send to the GPU
	/*struct sVertex
	{
		// POSITION
		// 2 floats == 8 bytes
		// Offset = 0
		float x, y;
		// COLOR0
		// 4 uint8_ts == 4 bytes
		// Offset = 8
		uint8_t b, g, r, a;	// Direct3D expects the byte layout of a color to be different from what you might expect
	};*/
	//IDirect3DVertexDeclaration9* s_vertexDeclaration = NULL;
	eae6320::Graphics::Mesh sMesh;
	eae6320::Graphics::Mesh FloorMesh;
	eae6320::Graphics::Mesh sMeshTriangle;
	eae6320::Graphics::Mesh transparentObject;

	eae6320::Graphics::Material sMaterialWood;
	eae6320::Graphics::Material sMaterialWoodTransparent;
	eae6320::Graphics::Material sMaterialMetal;
	eae6320::Graphics::Material sMaterialMetalTransparent;


	/*eae6320::Graphics::Effect sEffect;
	eae6320::Graphics::Effect sEffectTransparent;*/

	//std::vector <eae6320::Graphics::Renderable> RenderableList;



	// The vertex buffer holds the data for each vertex
	//IDirect3DVertexBuffer9* s_vertexBuffer = NULL;
	
	// An index buffer describes how to make triangles with the vertices
	// (i.e. it defines the vertex connectivity)
	//IDirect3DIndexBuffer9* s_indexBuffer = NULL;

	// The vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want
	//	IDirect3DVertexShader9* s_vertexShader = NULL;
	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	//IDirect3DPixelShader9* s_fragmentShader = NULL;
}

// Helper Function Declarations
//=============================

IDirect3DDevice9* eae6320::Graphics::getDirect3DDevice() {
	return s_direct3dDevice;
}

namespace
{
	bool CreateDevice();
	//bool CreateIndexBuffer();
	bool CreateInterface();
//	bool CreateVertexBuffer();
//	HRESULT GetVertexProcessingUsage( DWORD& o_usage );
//	bool LoadFragmentShader();
//	bool LoadVertexShader();
}

// Interface
//==========

bool eae6320::Graphics::Initialize( const HWND i_renderingWindow )
{
	s_renderingWindow = i_renderingWindow;

	// Initialize the interface to the Direct3D9 library
	if ( !CreateInterface() )
	{
		return false;
	}
	// Create an interface to a Direct3D device
	if ( !CreateDevice() )
	{
		goto OnError;
	}

	if (!eae6320::Graphics::LoadMesh(sMesh, "data/sphere.mesh"))
	{
		goto OnError;
	}

	if (!eae6320::Graphics::LoadMesh(FloorMesh, "data/floor.mesh"))
	{
		goto OnError;
	}
	if (!eae6320::Graphics::LoadMesh(transparentObject, "data/transparentObject.mesh"))
	{
		goto OnError;
	}

	eae6320::Graphics::LoadMaterial(sMaterialWoodTransparent, "data/woodTransparent.material");
	eae6320::Graphics::LoadMaterial(sMaterialWood, "data/wood.material");

	eae6320::Graphics::LoadMaterial(sMaterialMetal, "data/metal.material");
	eae6320::Graphics::LoadMaterial(sMaterialMetalTransparent, "data/metalTransparent.material");

	HRESULT result = s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	result = s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	result = s_direct3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		
	RenderableList.push_back( &renderableSquare);
	RenderableList.push_back(&renderableSphere);
	RenderableList.push_back(&renderableFloor);
	RenderableList.push_back(&renderableObject);

	renderableSphere.m_Material = sMaterialWood;
	renderableSphere.mMesh = sMesh;
	renderableSphere.m_position.x = -2.5;

	renderableSquare.m_Material = sMaterialMetal;
	renderableSquare.mMesh = sMesh;

	
	renderableFloor.m_position.y = -1;
	renderableFloor.m_Material = sMaterialWood;
	renderableFloor.mMesh = FloorMesh;

	renderableObject.mMesh = transparentObject;
	renderableObject.m_Material = sMaterialWoodTransparent;


	return true;

OnError:

	ShutDown();
	return false;
}

bool eae6320::Graphics::Clear() {
	{
		const D3DRECT* subRectanglesToClear = NULL;
		const DWORD subRectangleCount = 0;
		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ;
		D3DCOLOR clearColor;
		{
			// Black is usually used:
			clearColor = D3DCOLOR_XRGB(0, 0, 0);
		}
		const float noZBuffer = 1.0f;
		const DWORD noStencilBuffer = 0;
		HRESULT result = s_direct3dDevice->Clear(subRectangleCount, subRectanglesToClear,
			clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer);
		assert(SUCCEEDED(result));
	}
	return true;
}

bool eae6320::Graphics::BeginScene() {
	HRESULT result = s_direct3dDevice->BeginScene();
	assert(SUCCEEDED(result));
	return true;
}

bool eae6320::Graphics::EndScene() {

	HRESULT result = s_direct3dDevice->EndScene();
	assert(SUCCEEDED(result));
	return true;

}
bool  eae6320::Graphics::DisplayRenderedBuffer() {
	{
		const RECT* noSourceRectangle = NULL;
		const RECT* noDestinationRectangle = NULL;
		const HWND useDefaultWindow = NULL;
		const RGNDATA* noDirtyRegion = NULL;
		HRESULT result = s_direct3dDevice->Present(noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion);
		assert(SUCCEEDED(result));
	}
	return true;
}



//void eae6320::Graphics::Render()
//{
//	// Every frame an entirely new image will be created.
//	// Before drawing anything, then, the previous image will be erased
//	// by "clearing" the image buffer (filling it with a solid color)
//	{
//		const D3DRECT* subRectanglesToClear = NULL;
//		const DWORD subRectangleCount = 0;
//		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET;
//		D3DCOLOR clearColor;
//		{
//			// Black is usually used:
//			clearColor = D3DCOLOR_XRGB( 0, 0, 0 );
//		}
//		const float noZBuffer = 0.0f;
//		const DWORD noStencilBuffer = 0;
//		HRESULT result = s_direct3dDevice->Clear( subRectangleCount, subRectanglesToClear,
//			clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer );
//		assert( SUCCEEDED( result ) );
//	}
//
//	// The actual function calls that draw geometry must be made between paired calls to
//	// BeginScene() and EndScene()
//	{
//		
//		HRESULT result = s_direct3dDevice->BeginScene();
//		assert( SUCCEEDED( result ) );
//		{
//			
//			// Set the shaders
//			/*{
//				result = s_direct3dDevice->SetVertexShader( s_vertexShader );
//				assert( SUCCEEDED( result ) );
//				result = s_direct3dDevice->SetPixelShader( s_fragmentShader );
//				assert( SUCCEEDED( result ) );
//			}*/
//
//			/*
//			// Bind a specific vertex buffer to the device as a data source
//			{
//				// There can be multiple streams of data feeding the display adaptor at the same time
//				const unsigned int streamIndex = 0;
//				// It's possible to start streaming data in the middle of a vertex buffer
//				const unsigned int bufferOffset = 0;
//				// The "stride" defines how large a single vertex is in the stream of data
//				const unsigned int bufferStride = sizeof( sVertex );
//				result = s_direct3dDevice->SetStreamSource( streamIndex, s_vertexBuffer, bufferOffset, bufferStride );
//				assert( SUCCEEDED( result ) );
//			}
//			// Bind a specific index buffer to the device as a data source
//			{
//				result = s_direct3dDevice->SetIndices( s_indexBuffer );
//				assert( SUCCEEDED( result ) );
//			}
//			// Render objects from the current streams
//			{
//				// We are using triangles as the "primitive" type,
//				// and we have defined the vertex buffer as a triangle list
//				// (meaning that every triangle is defined by three vertices)
//				const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
//				// It's possible to start rendering primitives in the middle of the stream
//				const unsigned int indexOfFirstVertexToRender = 0;
//				const unsigned int indexOfFirstIndexToUse = 0;
//				// We are drawing a square
//				const unsigned int vertexCountToRender = 4;	// How vertices from the vertex buffer will be used?
//				const unsigned int primitiveCountToRender = 2;	// How many triangles will be drawn?
//				result = s_direct3dDevice->DrawIndexedPrimitive( primitiveType,
//					indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender,
//					indexOfFirstIndexToUse, primitiveCountToRender );
//				assert( SUCCEEDED( result ) );
//			}*/
//
//			SetEffect(sEffect);
//
//
//			for (int i = 0; i < RenderableList.size(); i++) {
//
//				//renderableSquare.mPositionOffset.x += 0.01;
//
//				eae6320::Graphics::SetDrawCallUniforms(RenderableList[i]->mEffect, reinterpret_cast<float*>(&RenderableList[i]->mPositionOffset));
//				eae6320::Graphics::DrawMesh(RenderableList[i]->mMesh);
//			}
//
//			////draw first triangle
//			//
//			//eae6320::Graphics::SetDrawCallUniforms(renderableTriangle1.mEffect, reinterpret_cast<float*>(&renderableTriangle1.mPositionOffset));
//			//eae6320::Graphics::DrawMesh(renderableTriangle1.mMesh);
//
//			////draw second triangle
//
//			//eae6320::Graphics::SetDrawCallUniforms(renderableTriangle2.mEffect, reinterpret_cast<float*>(&renderableTriangle2.mPositionOffset));
//			//eae6320::Graphics::DrawMesh(renderableTriangle2.mMesh);
//
//			////draw square
//			//eae6320::Graphics::SetDrawCallUniforms(renderableSquare.mEffect, reinterpret_cast<float*>(&renderableSquare.mPositionOffset));
//			//eae6320::Graphics::DrawMesh(renderableSquare.mMesh);
//					
//			
//		}
//		result = s_direct3dDevice->EndScene();
//		assert( SUCCEEDED( result ) );
//		
//		
//
//	}
//
//	// Everything has been drawn to the "back buffer", which is just an image in memory.
//	// In order to display it, the contents of the back buffer must be "presented"
//	// (to the front buffer)
//	{
//		const RECT* noSourceRectangle = NULL;
//		const RECT* noDestinationRectangle = NULL;
//		const HWND useDefaultWindow = NULL;
//		const RGNDATA* noDirtyRegion = NULL;
//		HRESULT result = s_direct3dDevice->Present( noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion );
//		assert( SUCCEEDED( result ) );
//	}
//}

bool eae6320::Graphics::ShutDown()
{
	bool wereThereErrors = false;

	if ( s_direct3dInterface )
	{
		if ( s_direct3dDevice )
		{
			if ( sMaterialWood.m_effect.s_vertexShader )
			{
				sMaterialWood.m_effect.s_vertexShader->Release();
				sMaterialWood.m_effect.s_vertexShader = NULL;
			}
			if (sMaterialWood.m_effect.s_fragmentShader )
			{
				sMaterialWood.m_effect.s_fragmentShader->Release();
				sMaterialWood.m_effect.s_fragmentShader = NULL;
			}

			if ( sMesh.s_vertexBuffer )
			{
				sMesh.s_vertexBuffer->Release();
				sMesh.s_vertexBuffer = NULL;
			}
			if ( sMesh.s_indexBuffer )
			{
				sMesh.s_indexBuffer->Release();
				sMesh.s_indexBuffer = NULL;
			}
			if ( sMesh.s_vertexDeclaration )
			{
				s_direct3dDevice->SetVertexDeclaration( NULL );
				sMesh.s_vertexDeclaration->Release();
				sMesh.s_vertexDeclaration = NULL;
			}

			//for second triangle mesh
			if (sMeshTriangle.s_vertexBuffer)
			{
				sMeshTriangle.s_vertexBuffer->Release();
				sMeshTriangle.s_vertexBuffer = NULL;
			}
			if (sMeshTriangle.s_indexBuffer)
			{
				sMeshTriangle.s_indexBuffer->Release();
				sMeshTriangle.s_indexBuffer = NULL;
			}
			if (sMeshTriangle.s_vertexDeclaration)
			{
				s_direct3dDevice->SetVertexDeclaration(NULL);
				sMeshTriangle.s_vertexDeclaration->Release();
				sMeshTriangle.s_vertexDeclaration = NULL;
			}

			s_direct3dDevice->Release();
			s_direct3dDevice = NULL;
		}

		s_direct3dInterface->Release();
		s_direct3dInterface = NULL;
	}
	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool CreateDevice()
	{
		const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
		const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
		const DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		D3DPRESENT_PARAMETERS presentationParameters = { 0 };
		{
			{
				const unsigned int useRenderingWindowDimensions = 0;
				presentationParameters.BackBufferWidth = useRenderingWindowDimensions;
				presentationParameters.BackBufferHeight = useRenderingWindowDimensions;
			}
			presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			presentationParameters.BackBufferCount = 1;
			presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			presentationParameters.hDeviceWindow = s_renderingWindow;
			presentationParameters.Windowed = TRUE;
			presentationParameters.EnableAutoDepthStencil = TRUE;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			presentationParameters.AutoDepthStencilFormat = D3DFMT_D16;// D3DFMT_16;
		}
		HRESULT result = s_direct3dInterface->CreateDevice( useDefaultDevice, useHardwareRendering,
			s_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &s_direct3dDevice );
		if ( SUCCEEDED( result ) )
		{
			return true;
		}
		else
		{
			eae6320::UserOutput::Print( "Direct3D failed to create a Direct3D9 device" );
			return false;
		}
	}

	bool CreateInterface()
	{
		// D3D_SDK_VERSION is #defined by the Direct3D header files,
		// and is just a way to make sure that everything is up-to-date
		s_direct3dInterface = Direct3DCreate9(D3D_SDK_VERSION);
		if (s_direct3dInterface)
		{
			return true;
		}
		else
		{
			eae6320::UserOutput::Print("DirectX failed to create a Direct3D9 interface");
			return false;
		}
	}

	/*bool CreateIndexBuffer()
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage( usage );
			if ( FAILED( result ) )
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
			bufferSize = triangleCount * vertexCountPerTriangle * sizeof( uint32_t );
			// We'll use 32-bit indices in this class to keep things simple
			// (i.e. every index will be a 32 bit unsigned integer)
			const D3DFORMAT format = D3DFMT_INDEX32;
			// Place the index buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* notUsed = NULL;
			const HRESULT result = s_direct3dDevice->CreateIndexBuffer( bufferSize, usage, format, useDefaultPool,
				&sMesh.s_indexBuffer, notUsed );
			if ( FAILED( result ) )
			{
				eae6320::UserOutput::Print( "Direct3D failed to create an index buffer" );
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
				const HRESULT result = sMesh.s_indexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>( &indexData ), useDefaultLockingBehavior );
				if ( FAILED( result ) )
				{
					eae6320::UserOutput::Print( "Direct3D failed to lock the index buffer" );
					return false;
				}
			}
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
				indexData[0] = 0;
				indexData[1] = 1;
				indexData[2] = 2;

				// Triangle 1
				indexData[3] = 0;
				indexData[4] = 2;
				indexData[5] = 3;

				// etc...
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = sMesh.s_indexBuffer->Unlock();
				if ( FAILED( result ) )
				{
					eae6320::UserOutput::Print( "Direct3D failed to unlock the index buffer" );
					return false;
				}
			}
		}

		return true;
	}
	*/

	
	/*
	bool CreateVertexBuffer()
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage( usage );
			if ( FAILED( result ) )
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
			HRESULT result = s_direct3dDevice->CreateVertexDeclaration( vertexElements, &sMesh.s_vertexDeclaration );
			if ( SUCCEEDED( result ) )
			{
				result = s_direct3dDevice->SetVertexDeclaration( sMesh.s_vertexDeclaration );
				if ( FAILED( result ) )
				{
					eae6320::UserOutput::Print( "Direct3D failed to set the vertex declaration" );
					return false;
				}
			}
			else
			{
				eae6320::UserOutput::Print( "Direct3D failed to create a Direct3D9 vertex declaration" );
				return false;
			}
		}

		// Create a vertex buffer
		{
			// We are drawing one square
			const unsigned int vertexCount = 4;	// What is the minimum number of vertices a square needs (so that no data is duplicated)?
			const unsigned int bufferSize = vertexCount * sizeof( sVertex );
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = NULL;
			const HRESULT result = s_direct3dDevice->CreateVertexBuffer( bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
				&sMesh.s_vertexBuffer, notUsed );
			if ( FAILED( result ) )
			{
				eae6320::UserOutput::Print( "Direct3D failed to create a vertex buffer" );
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
				const HRESULT result = sMesh.s_vertexBuffer->Lock( lockEntireBuffer, lockEntireBuffer,
					reinterpret_cast<void**>( &vertexData ), useDefaultLockingBehavior );
				if ( FAILED( result ) )
				{
					eae6320::UserOutput::Print( "Direct3D failed to lock the vertex buffer" );
					return false;
				}
			}
			// Fill the buffer
			{
				// You will need to fill in two pieces of information for each vertex:
				//	* 2 floats for the POSITION
				//	* 4 uint8_ts for the COLOR

				// The floats for POSITION are for the X and Y coordinates, like in Assignment 02.
				// The difference this time is that there should be fewer (because we are sharing data).
				
				// The uint8_ts for COLOR are "RGBA", where "RGB" stands for "Red Green Blue" and "A" for "Alpha".
				// Conceptually each of these values is a [0,1] value, but we store them as an 8-bit value to save space
				// (color doesn't need as much precision as position),
				// which means that the data we send to the GPU will be [0,255].
				// For now the alpha value should _always_ be 255, and so you will choose color by changing the first three RGB values.
				// To make white you should use (255, 255, 255), to make black (0, 0, 0).
				// To make pure red you would use the max for R and nothing for G and B, so (1, 0, 0).
				// Experiment with other values to see what happens!

				vertexData[0].x = 0.0f;
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
				vertexData[3].a = 255;

				//vertexData[1].x = EAE6320;
				// etc.
			}
			// The buffer must be "unlocked" before it can be used
			{
				const HRESULT result = sMesh.s_vertexBuffer->Unlock();
				if ( FAILED( result ) )
				{
					eae6320::UserOutput::Print( "Direct3D failed to unlock the vertex buffer" );
					return false;
				}
			}
		}

		return true;
	}
	*/

	/*
	HRESULT GetVertexProcessingUsage( DWORD& o_usage )
	{
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = s_direct3dDevice->GetCreationParameters( &deviceCreationParameters );
		if ( SUCCEEDED( result ) )
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				( D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING );
			o_usage = ( vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			eae6320::UserOutput::Print( "Direct3D failed to get the device's creation parameters" );
		}
		return result;
	}
	*/

	//bool LoadFragmentShader()
	//{
	//	// Load the source code from file and compile it
	//	ID3DXBuffer* compiledShader;
	//	{
	//		const char* sourceCodeFileName = "data/fragment.shader";
	//		//const D3DXMACRO* noMacros = NULL;
	//		const D3DXMACRO defines[] =
	//		{
	//			{ "EAE6320_PLATFORM_D3D", "1" },
	//			{ NULL, NULL }
	//		};
	//		ID3DXInclude* noIncludes = NULL;
	//		const char* entryPoint = "main";
	//		const char* profile = "ps_3_0";
	//		const DWORD noFlags = 0;
	//		ID3DXBuffer* errorMessages = NULL;
	//		ID3DXConstantTable** noConstants = NULL;
	//		HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
	//			&compiledShader, &errorMessages, noConstants );
	//		if ( SUCCEEDED( result ) )
	//		{
	//			if ( errorMessages )
	//			{
	//				errorMessages->Release();
	//			}
	//		}
	//		else
	//		{
	//			if ( errorMessages )
	//			{
	//				std::stringstream errorMessage;
	//				errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName
	//					<< ":\n" << reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//				errorMessages->Release();
	//			}
	//			else
	//			{
	//				std::stringstream errorMessage;
	//				errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName;
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//			}
	//			return false;
	//		}
	//	}
	//	// Create the fragment shader object
	//	bool wereThereErrors = false;
	//	{
	//		HRESULT result = s_direct3dDevice->CreatePixelShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
	//			&s_fragmentShader );
	//		if ( FAILED( result ) )
	//		{
	//			eae6320::UserOutput::Print( "Direct3D failed to create the fragment shader" );
	//			wereThereErrors = true;
	//		}
	//		compiledShader->Release();
	//	}
	//	return !wereThereErrors;
	//}

	//bool LoadVertexShader()
	//{
	//	// Load the source code from file and compile it
	//	ID3DXBuffer* compiledShader;
	//	{
	//		const char* sourceCodeFileName = "data/vertex.shader";
	//		const D3DXMACRO defines[] =
	//		{
	//			{ "EAE6320_PLATFORM_D3D", "1" },
	//			{ NULL, NULL }
	//		};
	//		//const D3DXMACRO* noMacros = NULL;
	//		ID3DXInclude* noIncludes = NULL;
	//		const char* entryPoint = "main";
	//		const char* profile = "vs_3_0";
	//		const DWORD noFlags = 0;
	//		ID3DXBuffer* errorMessages = NULL;
	//		ID3DXConstantTable** noConstants = NULL;
	//		HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
	//			&compiledShader, &errorMessages, noConstants );
	//		if ( SUCCEEDED( result ) )
	//		{
	//			if ( errorMessages )
	//			{
	//				errorMessages->Release();
	//			}
	//		}
	//		else
	//		{
	//			if ( errorMessages )
	//			{
	//				std::stringstream errorMessage;
	//				errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName
	//					<< ":\n" << reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//				errorMessages->Release();
	//			}
	//			else
	//			{
	//				std::stringstream errorMessage;
	//				errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName;
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//			}
	//			return false;
	//		}
	//	}
	//	// Create the vertex shader object
	//	bool wereThereErrors = false;
	//	{
	//		HRESULT result = s_direct3dDevice->CreateVertexShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
	//			&s_vertexShader );
	//		if ( FAILED( result ) )
	//		{
	//			eae6320::UserOutput::Print( "Direct3D failed to create the vertex shader" );
	//			wereThereErrors = true;
	//		}
	//		compiledShader->Release();
	//	}
	//	return !wereThereErrors;
	//}s
}

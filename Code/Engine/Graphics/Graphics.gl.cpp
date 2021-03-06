// Header Files
//=============

#include "Graphics.h"
#include "Effect.h"
#include "Mesh.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <sstream>
#include "../UserOutput/UserOutput.h"
#include "../Windows/Functions.h"
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#include "Renderable.h"
#include "Material.h"

// Static Data Initialization
//===========================
std::vector <eae6320::Graphics::Renderable*>  eae6320::Graphics::RenderableList;
eae6320::Graphics::Renderable renderableTriangle1;
eae6320::Graphics::Renderable renderableTriangle2;
eae6320::Graphics::Renderable renderableSquare;
eae6320::Graphics::Renderable renderableFloor;
eae6320::Graphics::Renderable renderableObject;
eae6320::Graphics::Renderable renderableSphere;
eae6320::Graphics::Renderable renderableCrosshair;
eae6320::Graphics::Renderable renderableGun;

namespace
{
	HWND s_renderingWindow = NULL;
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;

	// This struct determines the layout of the data that the CPU will send to the GPU
	struct sVertex
	{
		// POSITION
		// 2 floats == 8 bytes
		// Offset = 0
		float x, y;
		// COLOR0
		// 4 uint8_ts == 4 bytes
		// Offset = 8
		uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)


	};

	// A vertex array encapsulates both the vertex and index data as well as the vertex format
	//GLuint s_vertexArrayId = 0;
	eae6320::Graphics::Mesh sMesh;
	eae6320::Graphics::Mesh FloorMesh;
	eae6320::Graphics::Mesh sMeshTriangle;
	eae6320::Graphics::Mesh transparentObject;
	eae6320::Graphics::Mesh crosshair;
	eae6320::Graphics::Mesh gunMesh;


	eae6320::Graphics::Material sMaterialWood;
	eae6320::Graphics::Material sMaterialWoodTransparent;
	eae6320::Graphics::Material sMaterialMetal;
	eae6320::Graphics::Material sMaterialMetalTransparent;
	eae6320::Graphics::Material sMaterialCrosshair;
	eae6320::Graphics::Material sMaterialgun;

	// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".

	// A vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want

	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	//GLuint s_programId = 0;
}

// Helper Function Declarations
//=============================

namespace
{
	/*bool CreateProgram(eae6320::Graphics::Effect &i_Effect, const char * i_path);*/
	bool CreateRenderingContext();
//	bool CreateVertexArray();
	/*bool LoadAndAllocateShaderProgram( const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage );
	bool LoadFragmentShader( const GLuint i_programId );
	bool LoadVertexShader( const GLuint i_programId );*/


	/*struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};*/

	
}

// Interface
//==========

bool eae6320::Graphics::Initialize( const HWND i_renderingWindow )
{
	s_renderingWindow = i_renderingWindow;

	// Create an OpenGL rendering context
	if ( !CreateRenderingContext() )
	{
		goto OnError;
	}

	// Load any required OpenGL extensions
	{
		std::string errorMessage;
		if ( !OpenGlExtensions::Load( &errorMessage ) )
		{
			UserOutput::Print( errorMessage );
			goto OnError;
		}
	}

	// Initialize the graphics objects
	/*if ( !CreateVertexArray() )
	{
		goto OnError;
	}*/
	
	if (!eae6320::Graphics::LoadMesh(crosshair, "data/planeUP.mesh"))
	{
		goto OnError;
	}
	if (!eae6320::Graphics::LoadMesh(gunMesh, "data/gun.mesh"))
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
	eae6320::Graphics::LoadMaterial(sMaterialCrosshair, "data/crosshair.material");
	eae6320::Graphics::LoadMaterial(sMaterialgun, "data/gun.material");
	/*if (!eae6320::Graphics::LoadMesh(sMeshTriangle, "data/triangle.mesh"))
	{
		goto OnError;
	}*/
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);


	/*if ( !LoadEffect(sEffect, "data/mesheffect.effect") )
	{
		goto OnError;
	}

	if (!LoadEffect(sEffectTransparent, "data/transparentEffect.effect"))
	{
		goto OnError;
	}*/

	RenderableList.push_back(&renderableFloor);
	RenderableList.push_back(&renderableObject);
	RenderableList.push_back(&renderableSquare);
	RenderableList.push_back(&renderableSphere);
	RenderableList.push_back(&renderableCrosshair);
	RenderableList.push_back(&renderableGun);

	renderableSphere.m_Material = sMaterialWoodTransparent;
	renderableSphere.mMesh = sMesh;
	renderableSphere.m_position.x = -4.5;

	renderableSquare.m_Material = sMaterialMetalTransparent;
	renderableSquare.mMesh = sMesh;
	renderableSquare.m_position.z = -3.5f;

	renderableFloor.m_position.y = -2;
	renderableFloor.m_Material = sMaterialMetal;
	renderableFloor.mMesh = FloorMesh;

	renderableObject.mMesh = transparentObject;
	renderableObject.m_Material = sMaterialWood;
	renderableObject.m_position.y = 4;
	renderableObject.m_position.z = -10.5f;

	renderableCrosshair.mMesh = crosshair;
	renderableCrosshair.m_Material = sMaterialCrosshair;
	renderableCrosshair.m_position.z = 6.0f;

	renderableGun.mMesh = gunMesh;
	renderableGun.m_Material = sMaterialgun;
	renderableGun.m_position.y = -2.2f;

	//enabling backface culling 
	glEnable(GL_CULL_FACE);

	return true;

OnError:

	ShutDown();
	return false;
}
bool eae6320::Graphics::Clear() {
	{
		// Black is usually used
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		assert(glGetError() == GL_NO_ERROR);
		// In addition to the color, "depth" and "stencil" can also be cleared,
		// but for now we only care about color
		glClearDepth(1.0f);
		glDepthMask(GL_TRUE);
		const GLbitfield clearColor = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		glClear(clearColor);
		assert(glGetError() == GL_NO_ERROR);
	}
	return true;
}

bool eae6320::Graphics::BeginScene() {
	return true;
}

bool eae6320::Graphics::EndScene() {


	return true;
}
bool  eae6320::Graphics::DisplayRenderedBuffer() {
	{
		BOOL result = SwapBuffers(s_deviceContext);
		assert(result != FALSE);
	}
	return true;
}
//void eae6320::Graphics::Render()
//{
//	// Every frame an entirely new image will be created.
//	// Before drawing anything, then, the previous image will be erased
//	// by "clearing" the image buffer (filling it with a solid color)
//	{
//		// Black is usually used
//		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
//		assert( glGetError() == GL_NO_ERROR );
//		// In addition to the color, "depth" and "stencil" can also be cleared,
//		// but for now we only care about color
//		const GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
//		glClear( clearColor );
//		assert( glGetError() == GL_NO_ERROR );
//	}
//
//	// The actual function calls that draw geometry
//	{
//		// Set the vertex and fragment shaders
//		/*{
//			glUseProgram( s_programId );
//			assert( glGetError() == GL_NO_ERROR );
//		}*/
//
//		SetEffect(sEffect);
//		// Bind a specific vertex buffer to the device as a data source
//		
//		
//		// Render objects from the current streams
//		/*{
//			// We are using triangles as the "primitive" type,
//			// and we have defined the vertex buffer as a triangle list
//			// (meaning that every triangle is defined by three vertices)
//			const GLenum mode = GL_TRIANGLES;
//			// We'll use 32-bit indices in this class to keep things simple
//			// (i.e. every index will be a 32 bit unsigned integer)
//			const GLenum indexType = GL_UNSIGNED_INT;
//			// It is possible to start rendering in the middle of an index buffer
//			const GLvoid* const offset = 0;
//			// We are drawing a square
//			const GLsizei primitiveCountToRender = 2;	// How many triangles will be drawn?
//			const GLsizei vertexCountPerTriangle = 3;
//			const GLsizei vertexCountToRender = primitiveCountToRender * vertexCountPerTriangle;
//			glDrawElements( mode, vertexCountToRender, indexType, offset );
//			assert( glGetError() == GL_NO_ERROR );
//			{
//			glBindVertexArray(RenderableList[i]->mMesh.s_vertexArrayId);
//			assert(glGetError() == GL_NO_ERROR);
//			}
//		}*/
//
//		for (unsigned int i = 0; i < RenderableList.size(); i++) {
//			
//
//			eae6320::Graphics::SetDrawCallUniforms(RenderableList[i]->mEffect, reinterpret_cast<float*>(&RenderableList[i]->mPositionOffset));
//			eae6320::Graphics::DrawMesh(RenderableList[i]->mMesh);
//		}
//		
//		/*{
//			glBindVertexArray(sMeshTriangle.s_vertexArrayId);
//			assert(glGetError() == GL_NO_ERROR);
//		}
//		eae6320::Graphics::DrawMesh(sMeshTriangle);*/
//		
//	}
//
//	// Everything has been drawn to the "back buffer", which is just an image in memory.
//	// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
//	// (which is what the user sees)
//	{
//		BOOL result = SwapBuffers(s_deviceContext);
//		assert(result != FALSE);
//	}
//}

bool eae6320::Graphics::ShutDown()
{
	bool wereThereErrors = false;

	if ( s_openGlRenderingContext != NULL )
	{
		if (sMaterialCrosshair.m_effect.s_programId != 0 )
		{
			glDeleteProgram(sMaterialCrosshair.m_effect.s_programId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				UserOutput::Print( errorMessage.str() );
			}
			sMaterialCrosshair.m_effect.s_programId = 0;
		}
		if ( sMesh.s_vertexArrayId != 0 )
		{
			const GLsizei arrayCount = 1;
			glDeleteVertexArrays( arrayCount, &sMesh.s_vertexArrayId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex array: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				UserOutput::Print( errorMessage.str() );
			}
			sMesh.s_vertexArrayId = 0;
		}
		if (sMeshTriangle.s_vertexArrayId != 0)
		{
			const GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &sMeshTriangle.s_vertexArrayId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex array: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				UserOutput::Print(errorMessage.str());
			}
			sMeshTriangle.s_vertexArrayId = 0;
		}

		if ( wglMakeCurrent( s_deviceContext, NULL ) != FALSE )
		{
			if ( wglDeleteContext( s_openGlRenderingContext ) == FALSE )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to delete the OpenGL rendering context: " << GetLastWindowsError();
				UserOutput::Print( errorMessage.str() );
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to unset the current OpenGL rendering context: " << GetLastWindowsError();
			UserOutput::Print( errorMessage.str() );
		}
		s_openGlRenderingContext = NULL;
	}

	if ( s_deviceContext != NULL )
	{
		// The documentation says that this call isn't necessary when CS_OWNDC is used
		ReleaseDC( s_renderingWindow, s_deviceContext );
		s_deviceContext = NULL;
	}

	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Declarations
//=============================

namespace
{
	//bool CreateProgram(eae6320::Graphics::Effect &i_Effect, const char * i_path)
	//{
	//	// Create a program
	//	/*{
	//		i_Effect.s_programId = glCreateProgram();
	//		const GLenum errorCode = glGetError();
	//		if ( errorCode != GL_NO_ERROR )
	//		{
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to create a program: " <<
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//			eae6320::UserOutput::Print( errorMessage.str() );
	//			return false;
	//		}
	//		else if (i_Effect.s_programId == 0 )
	//		{
	//			eae6320::UserOutput::Print( "OpenGL failed to create a program" );
	//			return false;
	//		}
	//	}*/
	//	// Load and attach the shaders
	//	/*if ( !LoadVertexShader( s_programId ) )
	//	{
	//		return false;
	//	}
	//	if ( !LoadFragmentShader( s_programId ) )
	//	{
	//		return false;
	//	}*/

	//	eae6320::Graphics::LoadEffect(i_Effect, i_path);

	//	// Link the program
	//	{
	//		glLinkProgram(i_Effect.s_programId );
	//		GLenum errorCode = glGetError();
	//		if ( errorCode == GL_NO_ERROR )
	//		{
	//			// Get link info
	//			// (this won't be used unless linking fails
	//			// but it can be useful to look at when debugging)
	//			std::string linkInfo;
	//			{
	//				GLint infoSize;
	//				glGetProgramiv(i_Effect.s_programId, GL_INFO_LOG_LENGTH, &infoSize );
	//				errorCode = glGetError();
	//				if ( errorCode == GL_NO_ERROR )
	//				{
	//					eae6320::Graphics:: sLogInfo info( static_cast<size_t>( infoSize ) );
	//					GLsizei* dontReturnLength = NULL;
	//					glGetProgramInfoLog(i_Effect.s_programId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
	//					errorCode = glGetError();
	//					if ( errorCode == GL_NO_ERROR )
	//					{
	//						linkInfo = info.memory;
	//					}
	//					else
	//					{
	//						std::stringstream errorMessage;
	//						errorMessage << "OpenGL failed to get link info of the program: " <<
	//							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//						eae6320::UserOutput::Print( errorMessage.str() );
	//						return false;
	//					}
	//				}
	//				else
	//				{
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to get the length of the program link info: " <<
	//						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//					eae6320::UserOutput::Print( errorMessage.str() );
	//					return false;
	//				}
	//			}
	//			// Check to see if there were link errors
	//			GLint didLinkingSucceed;
	//			{
	//				glGetProgramiv(i_Effect.s_programId, GL_LINK_STATUS, &didLinkingSucceed );
	//				errorCode = glGetError();
	//				if ( errorCode == GL_NO_ERROR )
	//				{
	//					if ( didLinkingSucceed == GL_FALSE )
	//					{
	//						std::stringstream errorMessage;
	//						errorMessage << "The program failed to link:\n" << linkInfo;
	//						eae6320::UserOutput::Print( errorMessage.str() );
	//						return false;
	//					}
	//				}
	//				else
	//				{
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
	//						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//					eae6320::UserOutput::Print( errorMessage.str() );
	//					return false;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to link the program: " <<
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//			eae6320::UserOutput::Print( errorMessage.str() );
	//			return false;
	//		}
	//	}

	//	i_Effect.location_localToWorld = glGetUniformLocation(i_Effect.s_programId, "g_transform_localToWorld");

	//	if (i_Effect.location_localToWorld == -1) {
	//		std::stringstream errorMessage;
	//		errorMessage << "OpenGL failed to find the Uniform ";
	//		eae6320::UserOutput::Print(errorMessage.str());
	//	}

	//	i_Effect.location_worldToView = glGetUniformLocation(i_Effect.s_programId, "g_transform_worldToView");

	//	if (i_Effect.location_worldToView == -1) {
	//		std::stringstream errorMessage;
	//		errorMessage << "OpenGL failed to find the Uniform ";
	//		eae6320::UserOutput::Print(errorMessage.str());
	//	}

	//	i_Effect.location_viewToScreen = glGetUniformLocation(i_Effect.s_programId, "g_transform_viewToScreen");

	//	if (i_Effect.location_viewToScreen == -1) {
	//		std::stringstream errorMessage;
	//		errorMessage << "OpenGL failed to find the Uniform ";
	//		eae6320::UserOutput::Print(errorMessage.str());
	//	}

	//	return true;
	//}

	bool CreateRenderingContext()
	{
		// A "device context" can be thought of an abstraction that Windows uses
		// to represent the graphics adaptor used to display a given window
		s_deviceContext = GetDC( s_renderingWindow );
		if ( s_deviceContext == NULL )
		{
			eae6320::UserOutput::Print( "Windows failed to get the device context" );
			return false;
		}
		// Windows requires that an OpenGL "render context" is made for the window we want to use to render into
		{
			// Set the pixel format of the rendering window
			{
				PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
				{
					desiredPixelFormat.nSize = sizeof( PIXELFORMATDESCRIPTOR );
					desiredPixelFormat.nVersion = 1;

					desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
					desiredPixelFormat.cColorBits = 32;
					desiredPixelFormat.iLayerType = PFD_MAIN_PLANE ;
					desiredPixelFormat.cDepthBits = 16;
				}
				// Get the ID of the desired pixel format
				int pixelFormatId;
				{
					pixelFormatId = ChoosePixelFormat( s_deviceContext, &desiredPixelFormat );
					if ( pixelFormatId == 0 )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows couldn't choose the closest pixel format: " << eae6320::GetLastWindowsError();
						eae6320::UserOutput::Print( errorMessage.str() );
						return false;
					}
				}
				// Set it
				if ( SetPixelFormat( s_deviceContext, pixelFormatId, &desiredPixelFormat ) == FALSE )
				{
					std::stringstream errorMessage;
					errorMessage << "Windows couldn't set the desired pixel format: " << eae6320::GetLastWindowsError();
					eae6320::UserOutput::Print( errorMessage.str() );
					return false;
				}
			}
			// Create the OpenGL rendering context
			s_openGlRenderingContext = wglCreateContext( s_deviceContext );
			if ( s_openGlRenderingContext == NULL )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to create an OpenGL rendering context: " << eae6320::GetLastWindowsError();
				eae6320::UserOutput::Print( errorMessage.str() );
				return false;
			}
			// Set it as the rendering context of this thread
			if ( wglMakeCurrent( s_deviceContext, s_openGlRenderingContext ) == FALSE )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to set the current OpenGL rendering context: " << eae6320::GetLastWindowsError();
				eae6320::UserOutput::Print( errorMessage.str() );
				return false;
			}
		}

		return true;
	}

	/*bool CreateVertexArray()
	{
		bool wereThereErrors = false;
		GLuint vertexBufferId = 0;
		GLuint indexBufferId = 0;

		// Create a vertex array object and make it active
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays( arrayCount, &sMesh.s_vertexArrayId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindVertexArray( sMesh.s_vertexArrayId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex array: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex array ID: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}

		// Create a vertex buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers( bufferCount, &vertexBufferId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindBuffer( GL_ARRAY_BUFFER, vertexBufferId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex buffer: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex buffer ID: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}
		// Assign the data to the buffer
		{
			// We are drawing a square
			const unsigned int vertexCount = 4;	// What is the minimum number of vertices a square needs (so that no data is duplicated)?
			sVertex vertexData[vertexCount];
			// Fill in the data for the triangle
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

				vertexData[1].x = 1.0f;
				vertexData[1].y = 0.0f;
				// Green
				vertexData[1].r = 0;
				vertexData[1].g = 255;
				vertexData[1].b = 0;
				vertexData[1].a = 255;

				vertexData[2].x = 1.0f;
				vertexData[2].y = 1.0f;
				// Blue
				vertexData[2].r = 0;
				vertexData[2].g = 0;
				vertexData[2].b = 255;
				vertexData[2].a = 255;

				vertexData[3].x = 0.0f;
				vertexData[3].y = 1.0f;
				// Red
				vertexData[3].r = 255;
				vertexData[3].g = 0;
				vertexData[3].b = 0;
				vertexData[3].a = 255;

				//vertexData[1].x = EAE6320;
				// etc.
			}
			glBufferData( GL_ARRAY_BUFFER, vertexCount * sizeof( sVertex ), reinterpret_cast<GLvoid*>( vertexData ),
				// Our code will only ever write to the buffer
				GL_STATIC_DRAW );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the vertex buffer: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}
		// Initialize the vertex format
		{
			const GLsizei stride = sizeof( sVertex );
			GLvoid* offset = 0;

			// Position (0)
			// 2 floats == 8 bytes
			// Offset = 0
			{
				const GLuint vertexElementLocation = 0;
				const GLint elementCount = 2;
				const GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
				glVertexAttribPointer( vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride, offset );
				const GLenum errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					glEnableVertexAttribArray( vertexElementLocation );
					const GLenum errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						offset = reinterpret_cast<GLvoid*>( reinterpret_cast<uint8_t*>( offset ) + ( elementCount * sizeof( float ) ) );
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the POSITION vertex attribute: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the POSITION vertex attribute: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
			// Color (1)
			// 4 uint8_ts == 4 bytes
			// Offset = 8
			{
				const GLuint vertexElementLocation = 1;
				const GLint elementCount = 4;
				// Each element will be sent to the GPU as an unsigned byte in the range [0,255]
				// but these values should be understood as representing [0,1] values
				// and that is what the shader code will interpret them as
				// (in other words, we could change the values provided here in C code
				// to be floats and sent GL_FALSE instead and the shader code wouldn't need to change)
				const GLboolean normalized = GL_TRUE;
				glVertexAttribPointer( vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, normalized, stride, offset );
				const GLenum errorCode = glGetError();
				if ( errorCode == GL_NO_ERROR )
				{
					glEnableVertexAttribArray( vertexElementLocation );
					const GLenum errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						offset = reinterpret_cast<GLvoid*>( reinterpret_cast<uint8_t*>( offset ) + ( elementCount * sizeof( uint8_t ) ) );
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the COLOR0 vertex attribute: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the COLOR0 vertex attribute: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
		}

		// Create an index buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers( bufferCount, &indexBufferId );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBufferId );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the index buffer: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused index buffer ID: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}
		// Allocate space and copy the triangle data into the index buffer
		{
			// We are drawing a square
			const unsigned int triangleCount = 2;	// How many triangles does a square have?
			const unsigned int vertexCountPerTriangle = 3;
			uint32_t indexData[triangleCount * vertexCountPerTriangle];
			// Fill in the data for the triangle
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

			const GLsizeiptr bufferSize = triangleCount * vertexCountPerTriangle * sizeof( uint32_t );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, bufferSize, reinterpret_cast<const GLvoid*>( indexData ),
				// Our code will only ever write to the buffer
				GL_STATIC_DRAW );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the index buffer: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}

	OnExit:

		// Delete the buffer object
		// (the vertex array will hold a reference to it)
		if ( sMesh.s_vertexArrayId != 0 )
		{
			// Unbind the vertex array
			// (this must be done before deleting the vertex buffer)
			glBindVertexArray( 0 );
			const GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				if ( vertexBufferId != 0 )
				{
					// NOTE: If you delete the vertex buffer object here, as I recommend,
					// then gDEBugger won't know about it and you won't be able to examine the data.
					// If you find yourself in a situation where you want to see the buffer object data in gDEBugger
					// comment out this block of code temporarily
					// (doing this will cause a memory leak so make sure to restore the deletion code after you're done debugging).
					const GLsizei bufferCount = 1;
					glDeleteBuffers( bufferCount, &vertexBufferId );
					const GLenum errorCode = glGetError();
					if ( errorCode != GL_NO_ERROR )
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to delete the vertex buffer: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
					}
					vertexBufferId = 0;
				}
				if ( indexBufferId != 0 )
				{
					// NOTE: See the same comment above about deleting the vertex buffer object here and gDEBugger
					// holds true for the index buffer
					const GLsizei bufferCount = 1;
					glDeleteBuffers( bufferCount, &indexBufferId );
					const GLenum errorCode = glGetError();
					if ( errorCode != GL_NO_ERROR )
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "\nOpenGL failed to delete the index buffer: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
					}
					indexBufferId = 0;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to unbind the vertex array: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
			}
		}

		return !wereThereErrors;
	}*/

	//bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage)
	//{
	//	bool wereThereErrors = false;

	//	// Load the shader source from disk
	//	o_shader = NULL;
	//	HANDLE fileHandle = INVALID_HANDLE_VALUE;
	//	{
	//		// Open the file
	//		{
	//			const DWORD desiredAccess = FILE_GENERIC_READ;
	//			const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
	//			SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
	//			const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
	//			const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
	//			const HANDLE dontUseTemplateFile = NULL;
	//			fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
	//				useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
	//			if (fileHandle == INVALID_HANDLE_VALUE)
	//			{
	//				wereThereErrors = true;
	//				if (o_errorMessage)
	//				{
	//					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
	//					std::stringstream errorMessage;
	//					errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
	//					*o_errorMessage = errorMessage.str();
	//				}
	//				goto OnExit;
	//			}
	//		}
	//		// Get the file's size
	//		{
	//			LARGE_INTEGER fileSize_integer;
	//			if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
	//			{
	//				assert(fileSize_integer.QuadPart <= SIZE_MAX);
	//				o_size = static_cast<size_t>(fileSize_integer.QuadPart);
	//			}
	//			else
	//			{
	//				wereThereErrors = true;
	//				if (o_errorMessage)
	//				{
	//					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
	//					std::stringstream errorMessage;
	//					errorMessage << "Windows failed to get the size of shader: " << windowsErrorMessage;
	//					*o_errorMessage = errorMessage.str();
	//				}
	//				goto OnExit;
	//			}
	//			// Add an extra byte for a NULL terminator
	//			o_size += 1;
	//		}
	//		// Read the file's contents into temporary memory
	//		o_shader = malloc(o_size);
	//		if (o_shader)
	//		{
	//			DWORD bytesReadCount;
	//			OVERLAPPED* readSynchronously = NULL;
	//			if (ReadFile(fileHandle, o_shader, o_size,
	//				&bytesReadCount, readSynchronously) == FALSE)
	//			{
	//				wereThereErrors = true;
	//				if (o_errorMessage)
	//				{
	//					std::string windowsErrorMessage = eae6320::GetLastWindowsError();
	//					std::stringstream errorMessage;
	//					errorMessage << "Windows failed to read the contents of shader: " << windowsErrorMessage;
	//					*o_errorMessage = errorMessage.str();
	//				}
	//				goto OnExit;
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			if (o_errorMessage)
	//			{
	//				std::stringstream errorMessage;
	//				errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
	//				*o_errorMessage = errorMessage.str();
	//			}
	//			goto OnExit;
	//		}
	//		// Add the NULL terminator
	//		reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
	//	}

	//OnExit:

	//	if (wereThereErrors && o_shader)
	//	{
	//		free(o_shader);
	//		o_shader = NULL;
	//	}
	//	if (fileHandle != INVALID_HANDLE_VALUE)
	//	{
	//		if (CloseHandle(fileHandle) == FALSE)
	//		{
	//			if (!wereThereErrors && o_errorMessage)
	//			{
	//				std::string windowsError = eae6320::GetLastWindowsError();
	//				std::stringstream errorMessage;
	//				errorMessage << "Windows failed to close the shader file handle: " << windowsError;
	//				*o_errorMessage = errorMessage.str();
	//			}
	//			wereThereErrors = true;
	//		}
	//		fileHandle = INVALID_HANDLE_VALUE;
	//	}

	//	return !wereThereErrors;
	//}
	
	/*bool LoadAndAllocateShaderProgram( const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage )
	{
		bool wereThereErrors = false;

		// Load the shader source from disk
		o_shader = NULL;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		{
			// Open the file
			{
				const DWORD desiredAccess = FILE_GENERIC_READ;
				const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				const HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFile( i_path, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile );
				if ( fileHandle == INVALID_HANDLE_VALUE )
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the shader file: " <<
							eae6320::GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Get the file's size
			{
				LARGE_INTEGER fileSize_integer;
				if ( GetFileSizeEx( fileHandle, &fileSize_integer ) != FALSE )
				{
					assert( fileSize_integer.QuadPart <= SIZE_MAX );
					o_size = static_cast<size_t>( fileSize_integer.QuadPart );
				}
				else
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of shader: " <<
							eae6320::GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Read the file's contents into temporary memory
			o_shader = malloc( o_size );
			if ( o_shader )
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if ( ReadFile( fileHandle, o_shader, o_size,
					&bytesReadCount, readSynchronously ) == FALSE )
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of shader: " <<
							eae6320::GetLastWindowsError();
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					std::stringstream errorMessage;
					errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add the NULL terminator
			reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
		}

	OnExit:

		if ( wereThereErrors && o_shader )
		{
			free( o_shader );
			o_shader = NULL;
		}
		if ( fileHandle != INVALID_HANDLE_VALUE )
		{
			if ( CloseHandle( fileHandle ) == FALSE )
			{
				if ( !wereThereErrors && o_errorMessage )
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to close the shader file handle: " <<
						eae6320::GetLastWindowsError();
					*o_errorMessage = errorMessage.str();
				}
				wereThereErrors = true;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}*/

	//bool LoadFragmentShader( const GLuint i_programId )
	//{
	//	// Verify that compiling shaders at run-time is supported
	//	{
	//		GLboolean isShaderCompilingSupported;
	//		glGetBooleanv( GL_SHADER_COMPILER, &isShaderCompilingSupported );
	//		if ( !isShaderCompilingSupported )
	//		{
	//			eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
	//			return false;
	//		}
	//	}

	//	bool wereThereErrors = false;

	//	// Load the source code from file and set it into a shader
	//	GLuint fragmentShaderId = 0;
	//	void* shaderSource = NULL;
	//	{
	//		// Load the shader source code
	//		size_t fileSize;
	//		{
	//			const char* sourceCodeFileName = "data/fragment.shader";
	//			std::string errorMessage;
	//			if ( !LoadAndAllocateShaderProgram( sourceCodeFileName, shaderSource, fileSize, &errorMessage ) )
	//			{
	//				wereThereErrors = true;
	//				eae6320::UserOutput::Print( errorMessage );
	//				goto OnExit;
	//			}
	//		}
	//		// Generate a shader
	//		fragmentShaderId = glCreateShader( GL_FRAGMENT_SHADER );
	//		{
	//			const GLenum errorCode = glGetError();
	//			if ( errorCode != GL_NO_ERROR )
	//			{
	//				wereThereErrors = true;
	//				std::stringstream errorMessage;
	//				errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
	//					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//				goto OnExit;
	//			}
	//			else if ( fragmentShaderId == 0 )
	//			{
	//				wereThereErrors = true;
	//				eae6320::UserOutput::Print( "OpenGL failed to get an unused fragment shader ID" );
	//				goto OnExit;
	//			}
	//		}
	//		// Set the source code into the shader
	//		{
	//			const GLsizei shaderSourceCount = 3;
	//			const GLchar* shaderSources[] =
	//			{
	//				"#version 330// The version of GLSL to use must come first\n", 
	//				"#define EAE6320_PLATFORM_GL\n",
	//				reinterpret_cast<GLchar*>(shaderSource)
	//			};
	//			const GLint* sourcesAreNullTerminated = NULL;
	//			//const GLint length = static_cast<GLuint>( fileSize );
	//			glShaderSource( fragmentShaderId, shaderSourceCount, reinterpret_cast<GLchar**>( &shaderSources ), sourcesAreNullTerminated );
	//			const GLenum errorCode = glGetError();
	//			if ( errorCode != GL_NO_ERROR )
	//			{
	//				wereThereErrors = true;
	//				std::stringstream errorMessage;
	//				errorMessage << "OpenGL failed to set the fragment shader source code: " <<
	//					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//				eae6320::UserOutput::Print( errorMessage.str() );
	//				goto OnExit;
	//			}
	//		}
	//	}
	//	// Compile the shader source code
	//	{
	//		glCompileShader( fragmentShaderId );
	//		GLenum errorCode = glGetError();
	//		if ( errorCode == GL_NO_ERROR )
	//		{
	//			// Get compilation info
	//			// (this won't be used unless compilation fails
	//			// but it can be useful to look at when debugging)
	//			std::string compilationInfo;
	//			{
	//				GLint infoSize;
	//				glGetShaderiv( fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize );
	//				errorCode = glGetError();
	//				if ( errorCode == GL_NO_ERROR )
	//				{
	//					sLogInfo info( static_cast<size_t>( infoSize ) );
	//					GLsizei* dontReturnLength = NULL;
	//					glGetShaderInfoLog( fragmentShaderId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
	//					
	//					errorCode = glGetError();
	//					if ( errorCode == GL_NO_ERROR )
	//					{
	//						compilationInfo = info.memory;
	//					}
	//					else
	//					{
	//						wereThereErrors = true;
	//						std::stringstream errorMessage;
	//						errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
	//							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//						eae6320::UserOutput::Print( errorMessage.str() );
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
	//						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//					eae6320::UserOutput::Print( errorMessage.str() );
	//					goto OnExit;
	//				}
	//			}
	//			// Check to see if there were compilation errors
	//			GLint didCompilationSucceed;
	//			{
	//				glGetShaderiv( fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed );
	//				errorCode = glGetError();
	//				if ( errorCode == GL_NO_ERROR )
	//				{
	//					if ( didCompilationSucceed == GL_FALSE )
	//					{
	//						wereThereErrors = true;
	//						std::stringstream errorMessage;
	//						errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
	//						eae6320::UserOutput::Print( errorMessage.str() );
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
	//						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//					eae6320::UserOutput::Print( errorMessage.str() );
	//					goto OnExit;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//			eae6320::UserOutput::Print( errorMessage.str() );
	//			goto OnExit;
	//		}
	//	}
	//	// Attach the shader to the program
	//	{
	//		glAttachShader( i_programId, fragmentShaderId );
	//		const GLenum errorCode = glGetError();
	//		if ( errorCode != GL_NO_ERROR )
	//		{
	//			wereThereErrors = true;
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//			eae6320::UserOutput::Print( errorMessage.str() );
	//			goto OnExit;
	//		}
	//	}

	//OnExit:

	//	if ( fragmentShaderId != 0 )
	//	{
	//		// Even if the shader was successfully compiled
	//		// once it has been attached to the program we can (and should) delete our reference to it
	//		// (any associated memory that OpenGL has allocated internally will be freed
	//		// once the program is deleted)
	//		glDeleteShader( fragmentShaderId );
	//		const GLenum errorCode = glGetError();
	//		if ( errorCode != GL_NO_ERROR )
	//		{
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
	//				reinterpret_cast<const char*>( gluErrorString( errorCode ) );
	//			eae6320::UserOutput::Print( errorMessage.str() );
	//		}
	//		fragmentShaderId = 0;
	//	}
	//	if ( shaderSource != NULL )
	//	{
	//		free( shaderSource );
	//		shaderSource = NULL;
	//	}

	//	return !wereThereErrors;
	//}
	//bool LoadVertexShader(const GLuint i_programId)
	//{
	//	// Verify that compiling shaders at run-time is supported
	//	{
	//		GLboolean isShaderCompilingSupported;
	//		glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
	//		if (!isShaderCompilingSupported)
	//		{
	//			eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
	//			return false;
	//		}
	//	}

	//	bool wereThereErrors = false;

	//	// Load the source code from file and set it into a shader
	//	GLuint vertexShaderId = 0;
	//	void* shaderSource = NULL;
	//	{
	//		// Load the shader source code
	//		size_t fileSize;
	//		{
	//			const char* sourceCodeFileName = "data/vertex.shader";
	//			std::string errorMessage;
	//			if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
	//			{
	//				wereThereErrors = true;
	//				eae6320::UserOutput::Print(errorMessage);
	//				goto OnExit;
	//			}
	//		}
	//		// Generate a shader
	//		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	//		{
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				std::stringstream errorMessage;
	//				errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
	//					reinterpret_cast<const char*>(gluErrorString(errorCode));
	//				eae6320::UserOutput::Print(errorMessage.str());
	//				goto OnExit;
	//			}
	//			else if (vertexShaderId == 0)
	//			{
	//				wereThereErrors = true;
	//				eae6320::UserOutput::Print("OpenGL failed to get an unused vertex shader ID");
	//				goto OnExit;
	//			}
	//		}
	//		// Set the source code into the shader
	//		{
	//			const GLsizei shaderSourceCount = 3;
	//			const GLchar* shaderSources[] =
	//			{
	//				"#version 330// The version of GLSL to use must come first\n", 
	//				"#define EAE6320_PLATFORM_GL\n",
	//				reinterpret_cast<GLchar*>(shaderSource)
	//			};
	//			const GLint* sourcesAreNullTerminated = NULL;
	//			glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
	//			const GLenum errorCode = glGetError();
	//			if (errorCode != GL_NO_ERROR)
	//			{
	//				wereThereErrors = true;
	//				std::stringstream errorMessage;
	//				errorMessage << "OpenGL failed to set the vertex shader source code: " <<
	//					reinterpret_cast<const char*>(gluErrorString(errorCode));
	//				eae6320::UserOutput::Print(errorMessage.str());
	//				goto OnExit;
	//			}
	//		}
	//	}
	//	// Compile the shader source code
	//	{
	//		glCompileShader(vertexShaderId);
	//		GLenum errorCode = glGetError();
	//		if (errorCode == GL_NO_ERROR)
	//		{
	//			// Get compilation info
	//			// (this won't be used unless compilation fails
	//			// but it can be useful to look at when debugging)
	//			std::string compilationInfo;
	//			{
	//				GLint infoSize;
	//				glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					sLogInfo info(static_cast<size_t>(infoSize));
	//					GLsizei* dontReturnLength = NULL;
	//					glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
	//					errorCode = glGetError();
	//					if (errorCode == GL_NO_ERROR)
	//					{
	//						compilationInfo = info.memory;
	//					}
	//					else
	//					{
	//						wereThereErrors = true;
	//						std::stringstream errorMessage;
	//						errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
	//							reinterpret_cast<const char*>(gluErrorString(errorCode));
	//						eae6320::UserOutput::Print(errorMessage.str());
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
	//						reinterpret_cast<const char*>(gluErrorString(errorCode));
	//					eae6320::UserOutput::Print(errorMessage.str());
	//					goto OnExit;
	//				}
	//			}
	//			// Check to see if there were compilation errors
	//			GLint didCompilationSucceed;
	//			{
	//				glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
	//				errorCode = glGetError();
	//				if (errorCode == GL_NO_ERROR)
	//				{
	//					if (didCompilationSucceed == GL_FALSE)
	//					{
	//						wereThereErrors = true;
	//						std::stringstream errorMessage;
	//						errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
	//						eae6320::UserOutput::Print(errorMessage.str());
	//						goto OnExit;
	//					}
	//				}
	//				else
	//				{
	//					wereThereErrors = true;
	//					std::stringstream errorMessage;
	//					errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
	//						reinterpret_cast<const char*>(gluErrorString(errorCode));
	//					eae6320::UserOutput::Print(errorMessage.str());
	//					goto OnExit;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			wereThereErrors = true;
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
	//				reinterpret_cast<const char*>(gluErrorString(errorCode));
	//			eae6320::UserOutput::Print(errorMessage.str());
	//			goto OnExit;
	//		}
	//	}
	//	// Attach the shader to the program
	//	{
	//		glAttachShader(i_programId, vertexShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			wereThereErrors = true;
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
	//				reinterpret_cast<const char*>(gluErrorString(errorCode));
	//			eae6320::UserOutput::Print(errorMessage.str());
	//			goto OnExit;
	//		}
	//	}

	//OnExit:

	//	if (vertexShaderId != 0)
	//	{
	//		// Even if the shader was successfully compiled
	//		// once it has been attached to the program we can (and should) delete our reference to it
	//		// (any associated memory that OpenGL has allocated internally will be freed
	//		// once the program is deleted)
	//		glDeleteShader(vertexShaderId);
	//		const GLenum errorCode = glGetError();
	//		if (errorCode != GL_NO_ERROR)
	//		{
	//			std::stringstream errorMessage;
	//			errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
	//				reinterpret_cast<const char*>(gluErrorString(errorCode));
	//			eae6320::UserOutput::Print(errorMessage.str());
	//		}
	//		vertexShaderId = 0;
	//	}
	//	if (shaderSource != NULL)
	//	{
	//		free(shaderSource);
	//		shaderSource = NULL;
	//	}

	//	return !wereThereErrors;
	//}
}

/*	bool LoadVertexShader( const GLuint i_programId )
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv( GL_SHADER_COMPILER, &isShaderCompilingSupported );
			if ( !isShaderCompilingSupported )
			{
				eae6320::UserOutput::Print( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint vertexShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				const char* sourceCodeFileName = "data/vertex.shader";
				std::string errorMessage;
				if ( !LoadAndAllocateShaderProgram( sourceCodeFileName, shaderSource, fileSize, &errorMessage ) )
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print( errorMessage );
					goto OnExit;
				}
			}
			// Generate a shader
			vertexShaderId = glCreateShader( GL_VERTEX_SHADER );
			{
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
				else if ( vertexShaderId == 0 )
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print( "OpenGL failed to get an unused vertex shader ID" );
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 2;
				const GLchar* shaderSources[] =
				{
					"#define EAE6320_PLATFORM_GL\n",
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				//const GLint length = static_cast<GLuint>( fileSize );
				glShaderSource( vertexShaderId, shaderSourceCount, reinterpret_cast<GLchar**>( &shaderSource ), sourcesAreNullTerminated );
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex shader source code: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					eae6320::UserOutput::Print( errorMessage.str() );
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader( vertexShaderId );
			GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv( vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						sLogInfo info( static_cast<size_t>( infoSize ) );
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog( vertexShaderId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
						errorCode = glGetError();
						if ( errorCode == GL_NO_ERROR )
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
								reinterpret_cast<const char*>( gluErrorString( errorCode ) );
							eae6320::UserOutput::Print( errorMessage.str() );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv( vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						if ( didCompilationSucceed == GL_FALSE )
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
							eae6320::UserOutput::Print( errorMessage.str() );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						eae6320::UserOutput::Print( errorMessage.str() );
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader( i_programId, vertexShaderId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
				goto OnExit;
			}
		}

	OnExit:

		if ( vertexShaderId != 0 )
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader( vertexShaderId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				eae6320::UserOutput::Print( errorMessage.str() );
			}
			vertexShaderId = 0;
		}
		if ( shaderSource != NULL )
		{
			free( shaderSource );
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}
}*/

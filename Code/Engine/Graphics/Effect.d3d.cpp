#include "Effect.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "Graphics.h"
#include <sstream>
#include <assert.h>
#include <iostream>
#include <fstream>


namespace eae6320 {

	bool Graphics::SetDrawCallUniforms(const Effect &i_Effect, Math::cMatrix_transformation i_localToWorldTransform) {

		//float  floatArray[] = {0.0, 0.0};
		IDirect3DDevice9* direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		
		
		Math::cMatrix_transformation matrixWorldToView = Math::cMatrix_transformation::CreateWorldToViewTransform(Camera::getInstance().m_Orientation, Camera::getInstance().m_Position);

		Math::cMatrix_transformation matrixViewToScreen = Math::cMatrix_transformation::CreateViewToScreenTransform(Math::ConvertDegreesToRadians(60), 1.33f, 0.1f, 100);



		
//		HRESULT result = i_Effect.vertexShaderConstantTable->SetFloatArray(direct3dDevice, i_Effect.handle , i_floatArray, 2);

		HRESULT result = i_Effect.vertexShaderConstantTable->SetMatrixTranspose(direct3dDevice, i_Effect.handle_localToWorld, reinterpret_cast<const D3DXMATRIX*>(&i_localToWorldTransform));

		result = i_Effect.vertexShaderConstantTable->SetMatrixTranspose(direct3dDevice, i_Effect.handle_worldToView, reinterpret_cast<const D3DXMATRIX*>(&matrixWorldToView));

		result = i_Effect.vertexShaderConstantTable->SetMatrixTranspose(direct3dDevice, i_Effect.handle_viewToScreen, reinterpret_cast<const D3DXMATRIX*>(&matrixViewToScreen));

		return true;
	}

	void Graphics::SetEffect(const Effect &i_Effect) {

		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();

		HRESULT result = s_direct3dDevice->BeginScene();

		result = s_direct3dDevice->SetVertexShader(i_Effect.s_vertexShader);
		assert(SUCCEEDED(result));
		result = s_direct3dDevice->SetPixelShader(i_Effect.s_fragmentShader);
		assert(SUCCEEDED(result));

		//setting Render states
		uint8_t alpha = 1 << 0;
		uint8_t	depthTest = 1 << 1;
		uint8_t depthwrite = 1 << 2;
		uint8_t faceCulling = 1 << 3;


		if (i_Effect.renderstates & alpha) {
			s_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			s_direct3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			s_direct3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
		{
			s_direct3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}

		if (i_Effect.renderstates & depthTest) {
			s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			s_direct3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		}
		else
		{
			s_direct3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}

		if (i_Effect.renderstates & depthwrite) {
			s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
		{
			s_direct3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
		if (i_Effect.renderstates & faceCulling) {
			s_direct3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else
		{
			s_direct3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}

	Graphics::tUniformHandle Graphics::SetMaterialUniforms(const char * i_uniformName, Effect &i_Effect)
	{
		
		return i_Effect.fragmentShaderConstantTable->GetConstantByName(NULL, i_uniformName);
	}

	void Graphics::SetUniforms(Effect &i_Effect, Graphics::tUniformHandle i_UniformHandle, Graphics::eShaderType shaderType, float i_values[], uint8_t i_valueCounttoSet)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		i_Effect.vertexShaderConstantTable->SetFloatArray(s_direct3dDevice, i_UniformHandle, i_values, i_valueCounttoSet);
	}
	bool Graphics::CreateProgram(eae6320::Graphics::Effect &i_Effect)
	{
		return true;
	}
	bool Graphics::LinkProgram(eae6320::Graphics::Effect &i_Effect)
	{
		return true;
	}

	bool Graphics::LoadFragmentShader(Effect &i_Effect, const char * i_path)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		// Load the source code from file and compile it
		//ID3DXBuffer* compiledShader;
		
		//	const char* sourceCodeFileName = "data/fragment.shader";
			//const D3DXMACRO* noMacros = NULL;
		//	const D3DXMACRO defines[] =
		//	{
		//		{ "EAE6320_PLATFORM_D3D", "1" },
		//		{ NULL, NULL }
		//	};
		//	ID3DXInclude* noIncludes = NULL;
		/*	const char* entryPoint = "main";
			const char* profile = "ps_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;*/

			
			//HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
			//	&compiledShader, &errorMessages, &fragmentShaderConstantTable);
		//	D3DXHANDLE handle = fragmentShaderConstantTable->GetConstantByName(NULL, "g_position_offset");
		
			//load the shader file directly
			std::ifstream infile(i_path, std::ifstream::binary);

			// get size of file
			infile.seekg(0, infile.end);
			long size = static_cast<long>(infile.tellg());
			infile.seekg(0);

			// allocate memory for file content
			char* buffer = new char[size];

			// read content of infile
			infile.read(buffer, size);
			char * currentPointer = buffer;
			
			D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(buffer), &i_Effect.fragmentShaderConstantTable);

			
			//if (SUCCEEDED(result))
			//{
			//	if (errorMessages)
			//	{
			//		errorMessages->Release();
			//	//	fragmentShaderConstantTable->Release();
			//	}
			//}
			//else
			//{
			//	if (errorMessages)
			//	{
			//		std::stringstream errorMessage;
			//		errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName
			//			<< ":\n" << reinterpret_cast<char*>(errorMessages->GetBufferPointer());
			//		eae6320::UserOutput::Print(errorMessage.str());
			//		errorMessages->Release();
			//	}
			//	else
			//	{
			//		std::stringstream errorMessage;
			//		errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName;
			//		eae6320::UserOutput::Print(errorMessage.str());
			//	}
			//	return false;
			//}
		
		// Create the fragment shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(buffer),
				&i_Effect.s_fragmentShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the fragment shader");
				wereThereErrors = true;
			}
			//compiledShader->Release();
			delete[] buffer;
			infile.close();
		}
		
		return !wereThereErrors;
	}

	bool Graphics::LoadVertexShader(Effect &i_Effect, const char * i_path)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();

		// Load the source code from file and compile it
		//ID3DXBuffer* compiledShader;
		{
			/*const char* sourceCodeFileName = "data/vertex.shader";
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};*/
			//const D3DXMACRO* noMacros = NULL;
			/*ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "vs_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;*/
			
			/*HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, &i_Effect.vertexShaderConstantTable);*/

			
			//i_Effect.handle = i_Effect.vertexShaderConstantTable->GetConstantByName(NULL, "g_position_offset");
			/*if ( i_Effect.handle == -NULL) {
				std::stringstream errorMessage;
				errorMessage << "Direct3D failed to find the Uniform for vertex shader ";
				eae6320::UserOutput::Print(errorMessage.str());
			}*/
			std::ifstream infile(i_path, std::ifstream::binary);

			// get size of file
			infile.seekg(0, infile.end);
			long size = static_cast<long>(infile.tellg());
			infile.seekg(0);

			// allocate memory for file content
			char* buffer = new char[size];

			// read content of infile
			infile.read(buffer, size);
			char * currentPointer = buffer;
			
			D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(buffer), &i_Effect.vertexShaderConstantTable);
			i_Effect.handle_localToWorld = i_Effect.vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_localToWorld");
			if ( i_Effect.handle_localToWorld == -NULL) {
			std::stringstream errorMessage;
			errorMessage << "Direct3D failed to find the Uniform for vertex shader ";
			eae6320::UserOutput::Print(errorMessage.str());
			}

			i_Effect.handle_worldToView = i_Effect.vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_worldToView");
			if (i_Effect.handle_worldToView == -NULL) {
				std::stringstream errorMessage;
				errorMessage << "Direct3D failed to find the Uniform for vertex shader ";
				eae6320::UserOutput::Print(errorMessage.str());
			}

			i_Effect.handle_viewToScreen = i_Effect.vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_viewToScreen");
			if (i_Effect.handle_viewToScreen == -NULL) {
				std::stringstream errorMessage;
				errorMessage << "Direct3D failed to find the Uniform for vertex shader ";
				eae6320::UserOutput::Print(errorMessage.str());
			}
			/*if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				}
			}
			else
			{
				if (errorMessages)
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName
						<< ":\n" << reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					eae6320::UserOutput::Print(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName;
					eae6320::UserOutput::Print(errorMessage.str());
				}
				return false;
			}*/
		
		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(buffer),
				&i_Effect.s_vertexShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the vertex shader");
				wereThereErrors = true;
			}
			//compiledShader->Release();
			delete[] buffer;
			infile.close();
		}
		return !wereThereErrors;
		}
	}
}

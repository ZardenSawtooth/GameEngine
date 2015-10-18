#include "Effect.h"
#include <d3d9.h>
#include <d3dx9shader.h>
#include "Graphics.h"
#include <sstream>
#include <assert.h>

namespace eae6320 {

	bool Graphics::SetDrawCallUniforms(const Effect &i_Effect, float * i_floatArray) {

		//float  floatArray[] = {0.0, 0.0};
		IDirect3DDevice9* direct3dDevice = eae6320::Graphics::getDirect3DDevice();

		HRESULT result = i_Effect.vertexShaderConstantTable->SetFloatArray(direct3dDevice, i_Effect.handle , i_floatArray, 2);

		return true;
	}

	void Graphics::SetEffect(const Effect &i_Effect) {

		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();

		HRESULT result = s_direct3dDevice->BeginScene();

		result = s_direct3dDevice->SetVertexShader(i_Effect.s_vertexShader);
		assert(SUCCEEDED(result));
		result = s_direct3dDevice->SetPixelShader(i_Effect.s_fragmentShader);
		assert(SUCCEEDED(result));
	}

	bool Graphics::LoadFragmentShader(Effect &i_Effect, const char * i_path)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = "data/fragment.shader";
			//const D3DXMACRO* noMacros = NULL;
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "ps_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;

			ID3DXConstantTable* fragmentShaderConstantTable = NULL;
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, &fragmentShaderConstantTable);

		//	D3DXHANDLE handle = fragmentShaderConstantTable->GetConstantByName(NULL, "g_position_offset");
			if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				//	fragmentShaderConstantTable->Release();
				}
			}
			else
			{
				if (errorMessages)
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName
						<< ":\n" << reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					eae6320::UserOutput::Print(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName;
					eae6320::UserOutput::Print(errorMessage.str());
				}
				return false;
			}
		}
		// Create the fragment shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&i_Effect.s_fragmentShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the fragment shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool Graphics::LoadVertexShader(Effect &i_Effect, const char * i_path)
	{
		IDirect3DDevice9* s_direct3dDevice = eae6320::Graphics::getDirect3DDevice();

		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = "data/vertex.shader";
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};
			//const D3DXMACRO* noMacros = NULL;
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "vs_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			
			HRESULT result = D3DXCompileShaderFromFile(sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, &i_Effect.vertexShaderConstantTable);

			
			i_Effect.handle = i_Effect.vertexShaderConstantTable->GetConstantByName(NULL, "g_position_offset");
			if ( i_Effect.handle == -NULL) {
				std::stringstream errorMessage;
				errorMessage << "Direct3D failed to find the Uniform for vertex shader ";
				eae6320::UserOutput::Print(errorMessage.str());
			}


			if (SUCCEEDED(result))
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
			}
		}
		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&i_Effect.s_vertexShader);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to create the vertex shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}
}

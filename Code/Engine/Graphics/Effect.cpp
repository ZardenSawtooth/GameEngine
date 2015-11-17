#include "Effect.h"
#include "Graphics.h"
#include <assert.h>
#include <iostream>
#include <fstream>


namespace eae6320 {

	bool Graphics::LoadEffect(Effect &i_Effect, const char * i_path) {

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
		const char * FragmentPath = reinterpret_cast<const char *>(currentPointer);
		currentPointer += std::strlen(FragmentPath) + 1;
		const char * VertexPath = reinterpret_cast<const char *>(currentPointer);
		currentPointer += std::strlen(VertexPath) + 1;
		i_Effect.renderstates = *reinterpret_cast<uint8_t *>(currentPointer);

		// release dynamically-allocated memory
		

#if defined(EAE6320_PLATFORM_D3D)
		LoadFragmentShader(i_Effect, FragmentPath);
		LoadVertexShader(i_Effect, VertexPath);
#elif defined( EAE6320_PLATFORM_GL )
		LoadFragmentShader(i_Effect, FragmentPath);
		LoadVertexShader(i_Effect, VertexPath);

#endif
		delete[] buffer;
		infile.close();

		return true;

	}

	


}
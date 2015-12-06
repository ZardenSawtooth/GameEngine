#include "Material.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include "Effect.h"

void eae6320::Graphics::LoadMaterial(Material &i_Material, const char * i_path)
{
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

	//read effect
	const char * effectPath = reinterpret_cast<const char *>(currentPointer);
	currentPointer += std::strlen(effectPath) + 1;
	//set and load effect

	if (!LoadEffect(i_Material.m_effect, effectPath))
	{
		true;
	}


	uint32_t * numberOfUniforms = reinterpret_cast<uint32_t *>(currentPointer);
	i_Material.m_numberOfUniforms = *numberOfUniforms;
	currentPointer += sizeof(uint32_t);

	i_Material.uniformArray = new sUniformData[*numberOfUniforms];

	
	
 	for (unsigned int i = 0; i < *numberOfUniforms; i++) 
	{
		sUniformData * temp = reinterpret_cast<sUniformData *>(currentPointer);

		memcpy(&i_Material.uniformArray[i], temp, sizeof(sUniformData));

		//i_Material.uniformArray[i] = *reinterpret_cast<sUniformData *>(currentPointer);
		currentPointer += sizeof(sUniformData) + 1;
	}
	
	for (unsigned int i = 0; i < *numberOfUniforms; i++)
	{
		const char * tempNames = reinterpret_cast<const char *>(currentPointer);

		//sets the uniform handles for the material
		i_Material.uniformArray[i].uniformHandle = SetMaterialUniforms(tempNames, i_Material.m_effect);
		
		//const char * uniformNames = reinterpret_cast<const char *>(currentPointer);
		currentPointer += std::strlen(tempNames) + 1;
	}
	
}

void eae6320::Graphics::SetMaterial(Material &i_Material) {
	SetEffect(i_Material.m_effect);

	for (unsigned int i = 0; i < i_Material.m_numberOfUniforms; i++) {
		Graphics::SetUniforms(i_Material.m_effect ,i_Material.uniformArray[i].uniformHandle, i_Material.uniformArray[i].shaderType, i_Material.uniformArray[i].values, i_Material.uniformArray[i].valueCountToSet);
	}


}
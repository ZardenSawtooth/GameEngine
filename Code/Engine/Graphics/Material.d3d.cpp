#include "Material.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include "Effect.h"
#include "Graphics.h"

bool eae6320::Graphics::LoadTexture(Material &i_Material, const char * i_texturepath, const char * i_textureHandle) {

	IDirect3DDevice9* direct3dDevice = eae6320::Graphics::getDirect3DDevice();

	const unsigned int useDimensionsFromFile = D3DX_DEFAULT_NONPOW2;
	const unsigned int useMipMapsFromFile = D3DX_FROM_FILE;
	const DWORD staticTexture = 0;
	const D3DFORMAT useFormatFromFile = D3DFMT_FROM_FILE;
	const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
	const DWORD useDefaultFiltering = D3DX_DEFAULT;
	const D3DCOLOR noColorKey = 0;
	D3DXIMAGE_INFO* noSourceInfo = NULL;
	PALETTEENTRY* noColorPalette = NULL;
	const HRESULT result = D3DXCreateTextureFromFileEx(direct3dDevice, i_texturepath, useDimensionsFromFile, useDimensionsFromFile, useMipMapsFromFile,
		staticTexture, useFormatFromFile, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, noSourceInfo, noColorPalette,
		&i_Material.m_texture);

	D3DXHANDLE handle_sampler2D = i_Material.m_effect.fragmentShaderConstantTable->GetConstantByName(NULL, i_textureHandle);
	i_Material.samplerID = static_cast<DWORD> (i_Material.m_effect.fragmentShaderConstantTable->GetSamplerIndex(handle_sampler2D) );



	return true;
}

void eae6320::Graphics::SetTextures (Material &i_Material) {
	IDirect3DDevice9* direct3dDevice = eae6320::Graphics::getDirect3DDevice();

	const HRESULT result = direct3dDevice->SetTexture(i_Material.samplerID, i_Material.m_texture);
	if (!SUCCEEDED(result))
	{
		std::cerr << "Texture unable to set";
	}
	
}
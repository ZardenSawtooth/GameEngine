/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include <cstdlib>
#include <d3d9.h>

// Entry Point
//============

int main( int argumentCount, char** arguments )
{
	IDirect3D9* d3dInterface = Direct3DCreate9( D3D_SDK_VERSION );
	if ( d3dInterface )
	{
		// The interface was successfully created!
		d3dInterface->Release();
		d3dInterface = NULL;
		return EXIT_SUCCESS;
	}
	else
	{
		// The interface wasn't created :(
		return EXIT_FAILURE;
	}
}

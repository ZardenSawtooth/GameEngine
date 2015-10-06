#include "Effect.h"
#include "Graphics.h"
#include <assert.h>


namespace eae6320 {

	bool Graphics::LoadEffect(Effect &i_Effect, const char * i_FragmentPath, const char * i_VertexPath) {

		

#if defined(EAE6320_PLATFORM_D3D)
		LoadFragmentShader(i_Effect, i_FragmentPath);
		LoadVertexShader(i_Effect, i_VertexPath);
#elif defined( EAE6320_PLATFORM_GL )
		LoadFragmentShader(i_Effect, i_FragmentPath);
		LoadVertexShader(i_Effect, i_VertexPath);

#endif

		return true;

	}

	


}
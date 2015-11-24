#include "shaders.inc"

uniform float3 g_color_RGB;
uniform float g_color_alpha;

#if defined( EAE6320_PLATFORM_D3D )
	
	// Entry Point
	//============

	void main(

		// Input
		//======

		in float4 i_color : COLOR0,

		// Output
		//=======

		out float4 o_color : COLOR0

		)


#elif defined( EAE6320_PLATFORM_GL )
	
	// Input
	//======


	layout( location = 0 ) in vec4 i_color;

	// Output
	//=======

	out vec4 o_color;

	// Entry Point
	//============

	void main()
	#endif
	
	{
		// (where color is represented by 4 floats representing "RGBA" == "Red/Green/Blue/Alpha")
		{
			//o_color = i_color;
			//o_color.rgb *= g_color_RGB;
			o_color.rgb = i_color.rgb * g_color_RGB;
			o_color.a *= i_color.a * g_color_alpha;
			
		}
		
	}


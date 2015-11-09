#include "shaders.inc"

uniform float3 g_position_offset;

#if defined( EAE6320_PLATFORM_D3D )

	// Entry Point
	//============
	
	void main(

		// Input
		//======

		in const float3 i_position : POSITION,
		in const float4 i_color : COLOR,

		// Output
		//=======

		out float4 o_position : POSITION,
		out float4 o_color : COLOR

		)
	
#elif defined( EAE6320_PLATFORM_GL )

	// Input
	//======

	layout( location = 0 ) in vec3 i_position;
	layout( location = 1 ) in vec4 i_color;

	// Output
	//=======

	layout( location = 0 ) out vec4 o_color;

	// Entry Point
	//============

	void main()
	
#endif

	{
		// Calculate position
		{
			#if defined( EAE6320_PLATFORM_D3D )
			o_position
			#elif defined( EAE6320_PLATFORM_GL )
			gl_Position 
			#endif
			= vec4( i_position + g_position_offset, 1.0 );
			
		}
		{
			o_color = i_color;
		}
	}

	

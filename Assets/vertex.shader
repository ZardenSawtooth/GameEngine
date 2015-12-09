#include "shaders.inc"

uniform float4x4 g_transform_localToWorld;
uniform float4x4 g_transform_worldToView;
uniform float4x4 g_transform_viewToScreen;

#if defined( EAE6320_PLATFORM_D3D )

	// Entry Point
	//============
	
	void main(

		// Input
		//======

		in const float3 i_position_local : POSITION,
		in const float4 i_color : COLOR,
		in const float2 i_textureCoordinates : TEXCOORD0,

		// Output
		//=======

		out float4 O_POSITION_SCREEN : POSITION,
		out float4 o_color : COLOR,
		out float2 o_textureCoordinates : TEXCOORD0

		)
	
#elif defined( EAE6320_PLATFORM_GL )

	// Input
	//======

	layout( location = 0 ) in vec3 i_position_local;
	layout( location = 1 ) in vec4 i_color;
	layout( location = 2 ) in vec2 i_textureCoordinates;

	// Output
	//=======

	layout( location = 0 ) out vec4 o_color;
	layout( location = 1 ) out vec2 o_textureCoordinates;

	// Entry Point
	//============

	void main()
	
#endif

	// Calculate the position of this vertex on screen
{
    // The position stored in the vertex is in "local space",
    // meaning that it is relative to the center (or "origin", or "pivot")
    // of the mesh.
    // The graphics hardware needs the position of the vertex
    // in normalized device coordinates,
    // meaning where the position of the vertex should be drawn
    // on the screen.
    // This position that we need to output, then,
    // is the result of taking the original vertex in local space
    // and transforming it into "screen space".

    // Any matrix transformations that include translation
    // will operate on a float4 position,
    // which _must_ have 1 for the w value
    float4 position_world = Transform( float4( i_position_local, 1.0 ), g_transform_localToWorld );
    float4 position_view = Transform( position_world, g_transform_worldToView );
	
#if defined( EAE6320_PLATFORM_D3D )
    O_POSITION_SCREEN
#elif defined( EAE6320_PLATFORM_GL )
	gl_Position
#endif
	= Transform( position_view, g_transform_viewToScreen);
	{
		o_color = i_color;
		o_textureCoordinates = i_textureCoordinates;
	}
}


	

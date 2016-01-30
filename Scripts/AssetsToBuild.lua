return 
{
	Meshes = 
	{
		src = "",
		tar = "",
		tool = "MeshBuilder.exe",
		assets =
		{
			"ceiling.mesh",
			"cement.mesh",
			"floor.mesh",
			"lambert2.mesh",
			"lambert3.mesh",
			"metal.mesh",
			"railing.mesh",
			"walls.mesh",
		},
	},

	FragmentShaders =
	{
		src = "",
		tar = "",
		tool = "FragmentShaderBuilder.exe",
		assets = 
		{
			"fragment.shader",
			"transparent.shader",
		},
		dependencies = "shaders.inc",
	},

	VertexShaders = 
	{
		src = "",
		tar = "",
		tool = "VertexShaderBuilder.exe",
		assets = 
		{
			"vertex.shader",
		},
		dependencies = "shaders.inc",
	},

	Effects = 
	{
		src = "",
		tar = "",
		tool = "EffectBuilder.exe",
		assets = 
		{
			"mesheffect.effect",
			"transparentEffect.effect",
		},
	
	},
	Materials = 
	{
		src = "",
		tar = "",
		tool = "MaterialBuilder.exe",
		assets = 
		{
			"cement.material",
			"floor.material",
			"metal.material",
			"railing.material",
			"wall.material",
		},
	
	},
	Textures = 
	{
		src = "",
		tar = "",
		tool = "TextureBuilder.exe",
		assets = 
		{
			"cement_wall_D.png",
			"floor_D.png",
			"metal_brace_D.png",
			"railing_D.png",
			"wall_D.png",
		},
	},
}
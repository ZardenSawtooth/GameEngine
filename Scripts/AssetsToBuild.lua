return 
{
	Meshes = 
	{
		src = "",
		tar = "",
		tool = "MeshBuilder.exe",
		assets =
		{
			"floor.mesh",
			"torus.mesh",
			"transparentObject.mesh",
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
			"wood.material",
			"woodTransparent.material",
			"metal.material",
			"metalTransparent.material",
		},
	
	},
}
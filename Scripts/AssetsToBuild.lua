return 
{
	Meshes = 
	{
		src = "",
		tar = "",
		tool = "MeshBuilder.exe",
		assets =
		{
			"square.mesh",
			"triangle.mesh",
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
		},
	
	},
}
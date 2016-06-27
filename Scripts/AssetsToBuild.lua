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

	CollisionData = 
	{
		src = "",
		tar = "",
		tool = "CollisionDataBuilder.exe",
		assets =
		{
			"collisionData.mesh"
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
			"logo.png",
			"numbers.png",
			"bar.png"
		},
	},

	Audio = 
	{
		src = "Audio/",
		tar = "",
		tool = "GenericBuilder.exe",
		assets =
		{
			"GameMusic.wav",
			"EnemyPickUpFlag.wav",
			"PickUpFlag.wav",
			"ResetFlag.wav",
			"Scored.wav",
			"WalkingGround.wav",
			"WalkingUp.wav",
			"EnemyScored.wav",
			"Sprint.wav",
		},
	},
}
// Header Files
//=============

#include "cEffectBuilder.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>

// Interface
//==========

// Build
//------

bool eae6320::cEffectBuilder::Build(const std::vector<std::string>&)
{
	return LoadEffectData(m_path_source);
}

namespace {
	bool LoadAsset(const char* const i_path, std::ofstream& outfile);
	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_outfile);

}


namespace eae6320 {

	bool cEffectBuilder::LoadEffectData(const char * i_path) {

		std::ofstream outfile(m_path_target, std::ofstream::binary);

		const char* const path = i_path;
		if (!LoadAsset(path, outfile))
		{
			return false;
		}

		//#if defined( EAE6320_PLATFORM_D3D )
		//		//create index and vertex buffers
		//	//	Graphics::CreateVertexBuffer(vertexData, i_Mesh);
		//	//	Graphics::CreateIndexBuffer(indexData, i_Mesh);
		//#elif defined( EAE6320_PLATFORM_GL )
		//	//	Graphics::CreateVertexArray(i_Mesh, vertexData, indexData);
		//#endif

		

		//outfile.write(reinterpret_cast<const char *>(vertexData), sizeof(sVertex)*n);
		//outfile.write(reinterpret_cast<const char *>(indexData), sizeof(uint32_t)*3);

		//delete vertexdata and indexdata pointers
		
		outfile.close();

		return true;
	}
}

namespace
{

	bool LoadAsset(const char* i_path, std::ofstream& i_outfile)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				wereThereErrors = true;
				std::cerr << "Failed to create a new Lua state"
					"\n";
				goto OnExit;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path);
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				std::cerr << lua_tostring(luaState, -1);
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(luaState);
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;
						std::cerr << "Asset files must return a table (instead of a " <<
							luaL_typename(luaState, -1) << ")\n";
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::cerr << "Asset files must return a single table (instead of " <<
						returnedValueCount << " values)"
						"\n";
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::cerr << lua_tostring(luaState, -1);
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		if (!LoadTableValues(*luaState, i_outfile))
		{
			wereThereErrors = true;
		}

		// Pop the table
		lua_pop(luaState, 1);

	OnExit:

		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			assert(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_outfile)
	{
		bool wereThereErrors = false;

		const char* const keyfragment = "fragmentShaderPath";
		const char* const keyvertex = "vertexShaderPath";


		lua_pushstring(&io_luaState, keyfragment);
		lua_gettable(&io_luaState, -2);
		const char * fragmentpath = lua_tostring(&io_luaState, -1);
		i_outfile.write(reinterpret_cast<const char *>(fragmentpath), std::strlen(fragmentpath));
		i_outfile.write("\0", 1);
		lua_pop(&io_luaState, 1);


		lua_pushstring(&io_luaState, keyvertex);
		lua_gettable(&io_luaState, -2);
		const char * vertexpath = lua_tostring(&io_luaState, -1);
		i_outfile.write(reinterpret_cast<const char *>(fragmentpath), std::strlen(vertexpath));
		i_outfile.write("\0", 1);

		lua_pop(&io_luaState, 1);



		return true;
	}
}

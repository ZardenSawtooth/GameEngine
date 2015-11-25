// Header Files
//=============

#include "MaterialBuilder.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>

// Interface
//==========



// Build
//------


bool eae6320::MaterialBuilder::Build(const std::vector<std::string>&)
{
	return LoadMaterialData(m_path_source);
}

// Helper Function Declarations
//=============================

namespace
{
	bool LoadTableValues(lua_State& io_luaState, std::ofstream& i_outfile);

	bool LoadTableValues_Uniforms(lua_State& io_luaState, std::ofstream& i_outfile);

	bool LoadTableValues_uniformData(lua_State& io_luaState, std::ofstream& i_outfile);

	bool LoadAsset(const char* const i_path, std::ofstream& outfile);


}

namespace eae6320 {

	bool MaterialBuilder::LoadMaterialData(const char * i_path) {

		std::ofstream outfile(m_path_target, std::ofstream::binary);

		const char* const path = i_path;
		if (!LoadAsset(path, outfile))
		{
			return false;
		}

		

		

		//outfile.write(reinterpret_cast<const char *>(vertexData), sizeof(sVertex)*n);
		//outfile.write(reinterpret_cast<const char *>(indexData), sizeof(uint32_t)*3);
		outfile.close();

		return true;
	}
}



namespace
{

	bool LoadAsset(const char* i_path, std::ofstream& i_outfile)
	{
		//MessageBox(NULL, "", NULL, MB_OK);
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
				//std::cerr << lua_tostring(luaState, -1);
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
		//load effect name
		lua_pushstring(&io_luaState, "effect");
		lua_gettable(&io_luaState, -2);
		const char * effectname = lua_tostring(&io_luaState, -1);
		i_outfile.write(effectname, std::strlen(effectname));
		i_outfile.write("\0", 1);
		lua_pop(&io_luaState, 1);


		//load the uniforms
		if (!LoadTableValues_Uniforms(io_luaState, i_outfile))
		{
			return false;
		}


		/*if (!LoadTableValues_indices(io_luaState, i_outfile))
		{
			return false;
		}*/

		return true;
	}

	bool LoadTableValues_Uniforms(lua_State& io_luaState, std::ofstream& i_outfile)
	{
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		const char* const key = "uniformData";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		// It can be hard to remember where the stack is at
		// and how many values to pop.
		// One strategy I would suggest is to always call a new function
		// When you are at a new level:
		// Right now we know that we have an original table at -2,
		// and a new one at -1,
		// and so we _know_ that we always have to pop at least _one_
		// value before leaving this function
		// (to make the original table be back to index -1).
		// If we don't do any further stack manipulation in this function
		// then it becomes easy to remember how many values to pop
		// because it will always be one.
		// This is the strategy I'll take in this example
		// (look at the "OnExit" label):
		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_uniformData(io_luaState, i_outfile))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			goto OnExit;
		}

	OnExit:

		// Pop the uniform table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_uniformData(lua_State& io_luaState, std::ofstream& i_outfile)
	{
		bool wereThereErrors = false;
		// Right now the vertices table is at -1.
		// Every time the while() statement is executed it will be at -2
		// and the next key will be at -1.
		// Inside the block the table will be at -3,
		// the current key will be at -2,
		// and the value will be at -1.
		// (You may want to review LoadTableValues_allKeys()
		// in the ReadTopLevelTableValues example,
		// but remember that you don't need to know how to do this
		// for Assignment 03.)

		const char* const keyName = "name";
		
		
		uint32_t numberOfUniforms = luaL_len(&io_luaState, -1);
		i_outfile.write(reinterpret_cast<const char *>(&numberOfUniforms), sizeof(uint32_t));
		eae6320::sParameterToSet * parameters = new eae6320::sParameterToSet[numberOfUniforms];
		std::vector<const char *> names;
		for (unsigned int i = 1; i <= numberOfUniforms; i++) {

			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			{
				//name of uniform
				lua_pushstring(&io_luaState, keyName);
				lua_gettable(&io_luaState, -2);
				names.push_back(lua_tostring(&io_luaState, -1));
				lua_pop(&io_luaState, 1);

				//shader type
				lua_pushstring(&io_luaState, "shadertype");
				lua_gettable(&io_luaState, -2);
				const char * type = lua_tostring(&io_luaState, -1);

				if ( strcmp(type, "fragment") == 0)
					parameters[i - 1].shaderType = fragment;
				else
					parameters[i - 1].shaderType = vertex;

				lua_pop(&io_luaState, 1);

				//values

				lua_pushstring(&io_luaState, "values");
				lua_gettable(&io_luaState, -2);
				uint32_t numberofValues = luaL_len(&io_luaState, -1);
				parameters[i - 1].valueCountToSet = numberofValues;
				

				for (unsigned int j = 1; j <= numberofValues; j++) 
				{
					lua_pushinteger(&io_luaState, j);
					lua_gettable(&io_luaState, -2);

					parameters[i-1].values[j-1] = static_cast<float> (lua_tonumber(&io_luaState, -1));
					//std::cerr << "\nfloat value" << parameters[i - 1].values[j - 1];
					lua_pop(&io_luaState, 1);
				}
				lua_pop(&io_luaState, 1);
			}

			lua_pop(&io_luaState, 1);
		}
		for (unsigned int i = 0; i < numberOfUniforms; i++)
		{
			//char* temp = new char[sizeof(eae6320::sParameterToSet)];
			//memcpy(temp, reinterpret_cast<void*>(&parameters[i]), sizeof(eae6320::sParameterToSet));
			i_outfile.write(reinterpret_cast<char *>(&parameters[i]), sizeof(eae6320::sParameterToSet) );
			i_outfile.write("\0", 1);
		}
		for (unsigned int i = 0; i < numberOfUniforms; i++) 
		{
			i_outfile.write(names[i], std::strlen(names[i]));
			i_outfile.write("\0", 1);
		}

		return !wereThereErrors;

		return true;
	}
}


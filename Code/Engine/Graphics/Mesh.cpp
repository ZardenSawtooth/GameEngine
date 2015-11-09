#include "Mesh.h"
#include <iostream>
#include <assert.h>
#include <fstream>


/*
eae6320::Graphics::sVertex* vertexData = NULL;
uint32_t* indexData = NULL;

// Helper Function Declarations
//=============================

namespace
{
	bool LoadTableValues(lua_State& io_luaState);

	bool LoadTableValues_vertices(lua_State& io_luaState);
	bool LoadTableValues_vertexData(lua_State& io_luaState);
	bool GetPositionValues(lua_State& io_luaState, unsigned int i_index);
	bool GetColorValues(lua_State& io_luaState, unsigned int i_index);

	bool LoadTableValues_indices(lua_State& io_luaState);
	bool LoadTableValues_indexData(lua_State& io_luaState);


	bool LoadAsset(const char* const i_path);
}
*/
namespace eae6320 {
	
	bool Graphics::LoadMesh( Mesh &i_Mesh, const char * i_path) {
		/*const char* const path = i_path;
		if (!LoadAsset(path))
		{
			return false;
		}*/
		//load file to read from
		std::ifstream infile(i_path, std::ifstream::binary);

		// get size of file
		infile.seekg(0, infile.end);
		long size = static_cast<long>(infile.tellg());
		infile.seekg(0);

		// allocate memory for file content
		char* buffer = new char[size];

		// read content of infile
		infile.read(buffer, size);
		char * currentPointer = buffer;
		uint32_t vertexCount = *reinterpret_cast<uint32_t*>(currentPointer);
		currentPointer += sizeof(uint32_t);
		sVertex * vertexData = reinterpret_cast<sVertex*>(currentPointer);
		currentPointer = currentPointer + (sizeof(sVertex) * vertexCount);
		uint32_t indexCount = *reinterpret_cast<uint32_t*>(currentPointer);
		currentPointer += sizeof(uint32_t);
		uint32_t * indexData = reinterpret_cast<uint32_t *>(currentPointer);
		

#if defined( EAE6320_PLATFORM_D3D )
		//create index and vertex buffers
		Graphics::CreateVertexBuffer(vertexCount, vertexData, i_Mesh);
		Graphics::CreateIndexBuffer(indexCount,  indexData, i_Mesh);
#elif defined( EAE6320_PLATFORM_GL )
		Graphics::CreateVertexArray(i_Mesh, vertexData, indexData, vertexCount, indexCount);
#endif

		// release dynamically-allocated memory
		delete[] buffer;
		infile.close();


		return true;
	}
}

/*

namespace
{

	bool LoadAsset(const char* i_path)
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
		if (!LoadTableValues(*luaState))
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

	bool LoadTableValues(lua_State& io_luaState)
	{
		if (!LoadTableValues_vertices(io_luaState))
		{
			return false;
		}
		if (!LoadTableValues_indices(io_luaState))
		{
			return false;
		}

		return true;
	}

	bool LoadTableValues_vertices(lua_State& io_luaState)
	{
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		const char* const key = "vertices";
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
			if (!LoadTableValues_vertexData(io_luaState))
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

		// Pop the vertices table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_indices(lua_State& io_luaState)
	{
		bool wereThereErrors = false;

		// Right now the asset table is at -1.
		// After the following table operation it will be at -2
		// and the "textures" table will be at -1:
		const char* const key = "indices";
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
			if (!LoadTableValues_indexData(io_luaState))
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

		// Pop the vertices table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTableValues_vertexData(lua_State& io_luaState)
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

		const char* const keypos = "position";
		const char* const colpos = "color";
		int numberOfVertices = luaL_len(&io_luaState, -1);

		vertexData = new eae6320::Graphics::sVertex[numberOfVertices];

		for (int i = 1; i <= numberOfVertices; i++) {

			//push vertexData table
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			//push positions 
			lua_pushstring(&io_luaState, keypos);
			lua_gettable(&io_luaState, -2);

		

			//get values of positions
			if (!GetPositionValues(io_luaState, i-1))
				return false;

			//pop position values
			lua_pop(&io_luaState, 1);

			//push color
			lua_pushstring(&io_luaState, colpos);
			lua_gettable(&io_luaState, -2);
			

			// Gets color values from current table
			if (!GetColorValues(io_luaState, i-1))
				return false;

			//pops the color values
			lua_pop(&io_luaState, 1);

			// Pop the vertexData table
			lua_pop(&io_luaState, 1);

		}

	
//	OnExit:
	
		return !wereThereErrors;

		return true;
	}

	bool GetPositionValues(lua_State& io_luaState, unsigned int i_index) 
	{
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].x = static_cast<float> ( lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);
		
		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].y = static_cast<float> ( lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);

		return true;
	}

	bool GetColorValues(lua_State& io_luaState, unsigned int i_index)
	{
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].r = static_cast<uint8_t> (255 * lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].g = static_cast<uint8_t> (255 * lua_tonumber(&io_luaState, -1));
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 3);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].b = static_cast<uint8_t> (255 * lua_tonumber(&io_luaState, -1));
		lua_pop(&io_luaState, 1);

		lua_pushinteger(&io_luaState, 4);
		lua_gettable(&io_luaState, -2);

		vertexData[i_index].a = static_cast<uint8_t> (255 * lua_tonumber(&io_luaState, -1));
		lua_pop(&io_luaState, 1);

		return true;
	}

	bool LoadTableValues_indexData(lua_State& io_luaState)
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

		int numberOfindices = luaL_len(&io_luaState, -1);

		indexData = new uint32_t[numberOfindices * 3];
		int indexCount = -1;
		for (int i = 1; i <= numberOfindices; i++) {

			//push indexData table
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			

#if defined( EAE6320_PLATFORM_D3D )
			//push index first value 
			lua_pushinteger(&io_luaState, 1);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t> (  lua_tointeger(&io_luaState, -1) );

			//pop first value
			lua_pop(&io_luaState, 1);

			//push index second value 
			lua_pushinteger(&io_luaState, 3);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t> ( lua_tointeger(&io_luaState, -1) );

			//pop second value
			lua_pop(&io_luaState, 1);

			//push index third value 
			lua_pushinteger(&io_luaState, 2);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t>( lua_tointeger(&io_luaState, -1) );

			//pop third value
			lua_pop(&io_luaState, 1);

#elif defined(EAE6320_PLATFORM_GL)

			//push index first value 
			lua_pushinteger(&io_luaState, 1);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t> (lua_tointeger(&io_luaState, -1) );

			//pop first value
			lua_pop(&io_luaState, 1);

			//push index second value 
			lua_pushinteger(&io_luaState, 2);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t>( lua_tointeger(&io_luaState, -1));

			//pop second value
			lua_pop(&io_luaState, 1);

			//push index third value 
			lua_pushinteger(&io_luaState, 3);
			lua_gettable(&io_luaState, -2);

			indexData[++indexCount] = static_cast<uint32_t> ( lua_tointeger(&io_luaState, -1) );

			//pop third value
			lua_pop(&io_luaState, 1);


#endif


			// Pop the vertexData table
			lua_pop(&io_luaState, 1);

		}


//	OnExit:

		return !wereThereErrors;

		return true;
	}
}*/
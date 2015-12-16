

#include "../Windows/Functions.h"
#include "Effect.h"
#include "Graphics.h"
#include <sstream>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <assert.h>




namespace eae6320 {

	bool Graphics::SetDrawCallUniforms(const Effect &i_Effect, Math::cMatrix_transformation i_localToWorldTransform) {

		//float  floatArray[] = { 0.0, 0.0 };
		//glUniform2fv(i_Effect.location, 1, i_floatArray);
		Math::cMatrix_transformation matrix;
		const GLboolean dontTranspose = false; // Matrices are already in the correct format
		const GLsizei uniformCountToSet = 1;

		Math::cMatrix_transformation matrixWorldToView = Math::cMatrix_transformation::CreateWorldToViewTransform(Camera::getInstance().m_Orientation, Camera::getInstance().m_Position);

		//Math::cMatrix_transformation matrixWorldToView = Math::cMatrix_transformation::CreateWorldToViewTransform(Math::cQuaternion::cQuaternion(), Math::cVector(0, 0, 10));
		Math::cMatrix_transformation matrixViewToScreen = Math::cMatrix_transformation::CreateViewToScreenTransform(Math::ConvertDegreesToRadians(60), 1.33f, 0.1f, 100);

		glUniformMatrix4fv(i_Effect.location_localToWorld , uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&i_localToWorldTransform));
		glUniformMatrix4fv(i_Effect.location_worldToView, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&matrixWorldToView));
		glUniformMatrix4fv(i_Effect.location_viewToScreen, uniformCountToSet, dontTranspose, reinterpret_cast<const GLfloat*>(&matrixViewToScreen));
		return true;

	}

	bool Graphics::CreateProgram(eae6320::Graphics::Effect &i_Effect) 
	{
		{
			i_Effect.s_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				return false;
			}
			else if (i_Effect.s_programId == 0)
			{
				eae6320::UserOutput::Print("OpenGL failed to create a program");
				return false;
			}
		}
		
	}

	Graphics::tUniformHandle Graphics::SetMaterialUniforms(const char * i_uniformName, Effect &i_Effect )
	{
		return glGetUniformLocation(i_Effect.s_programId, i_uniformName);
	}

	void Graphics::SetUniforms(Effect &i_Effect, Graphics::tUniformHandle i_UniformHandle, Graphics::eShaderType shaderType, float i_values[], uint8_t i_valueCounttoSet)
	{
		//glUniform2fv(i_Effect.location, 1, i_floatArray);
		switch (i_valueCounttoSet)
		{
		case 1:
			glUniform1fv(i_UniformHandle, 1, i_values);
			break;
		case 2:
			glUniform2fv(i_UniformHandle, 1, i_values);
			break;
		case 3:
			glUniform3fv(i_UniformHandle, 1, i_values);
			break;
		case 4:
			glUniform4fv(i_UniformHandle, 1, i_values);
			break;
		}
	}

	bool Graphics::LinkProgram(eae6320::Graphics::Effect &i_Effect)
	{
		{
			glLinkProgram(i_Effect.s_programId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get link info
				// (this won't be used unless linking fails
				// but it can be useful to look at when debugging)
				std::string linkInfo;
				{
					GLint infoSize;
					glGetProgramiv(i_Effect.s_programId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						eae6320::Graphics::sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetProgramInfoLog(i_Effect.s_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							linkInfo = info.memory;
						}
						else
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get link info of the program: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							eae6320::UserOutput::Print(errorMessage.str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the program link info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}
				// Check to see if there were link errors
				GLint didLinkingSucceed;
				{
					glGetProgramiv(i_Effect.s_programId, GL_LINK_STATUS, &didLinkingSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didLinkingSucceed == GL_FALSE)
						{
							std::stringstream errorMessage;
							errorMessage << "The program failed to link:\n" << linkInfo;
							eae6320::UserOutput::Print(errorMessage.str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						return false;
					}
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to link the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				return false;
			}
		}

		i_Effect.location_localToWorld = glGetUniformLocation(i_Effect.s_programId, "g_transform_localToWorld");

		if (i_Effect.location_localToWorld == -1) {
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to find the Uniform ";
			eae6320::UserOutput::Print(errorMessage.str());
		}

		i_Effect.location_worldToView = glGetUniformLocation(i_Effect.s_programId, "g_transform_worldToView");

		if (i_Effect.location_worldToView == -1) {
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to find the Uniform ";
			eae6320::UserOutput::Print(errorMessage.str());
		}

		i_Effect.location_viewToScreen = glGetUniformLocation(i_Effect.s_programId, "g_transform_viewToScreen");

		if (i_Effect.location_viewToScreen == -1) {
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to find the Uniform ";
			eae6320::UserOutput::Print(errorMessage.str());
		}
		return true;
	}


	void Graphics::SetEffect(const Effect &i_Effect) {

		//Calls to set shaders
		{
			glUseProgram(i_Effect.s_programId);
			assert(glGetError() == GL_NO_ERROR);

			uint8_t alpha = 1 << 0;
			uint8_t	depthTest = 1 << 1;
			uint8_t depthwrite = 1 << 2;
			uint8_t faceCulling = 1 << 3;


			if (i_Effect.renderstates & alpha) 
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			if (i_Effect.renderstates & depthTest) 
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LEQUAL);
			}
			else
			{
				glDisable(GL_DEPTH_TEST);
			}

			if (i_Effect.renderstates & depthwrite) 
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}
			if (i_Effect.renderstates & faceCulling) {
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}
		}

	}

	bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage)
	{
		bool wereThereErrors = false;

		// Load the shader source from disk
		o_shader = NULL;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		{
			// Open the file
			{
				const DWORD desiredAccess = FILE_GENERIC_READ;
				const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				const HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Get the file's size
			{
				LARGE_INTEGER fileSize_integer;
				if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
				{
					assert(fileSize_integer.QuadPart <= SIZE_MAX);
					o_size = static_cast<size_t>(fileSize_integer.QuadPart);
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of shader: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
				// Add an extra byte for a NULL terminator
				o_size += 1;
			}
			// Read the file's contents into temporary memory
			o_shader = malloc(o_size);
			if (o_shader)
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if (ReadFile(fileHandle, o_shader, o_size,
					&bytesReadCount, readSynchronously) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of shader: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add the NULL terminator
			reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
		}

	OnExit:

		if (wereThereErrors && o_shader)
		{
			free(o_shader);
			o_shader = NULL;
		}
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
				if (!wereThereErrors && o_errorMessage)
				{
					std::string windowsError = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to close the shader file handle: " << windowsError;
					*o_errorMessage = errorMessage.str();
				}
				wereThereErrors = true;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	

	bool Graphics::LoadFragmentShader(Effect &i_Effect, const char * i_path)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint fragmentShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				const char* sourceCodeFileName = i_path;
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
				else if (fragmentShaderId == 0)
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print("OpenGL failed to get an unused fragment shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				//const GLint length = static_cast<GLuint>( fileSize );
				glShaderSource(fragmentShaderId, shaderSourceCount, reinterpret_cast<GLchar**>(&shaderSources), sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the fragment shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(fragmentShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);

						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(i_Effect.s_programId, fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		if (fragmentShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
			}
			fragmentShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}
	bool Graphics::LoadVertexShader(Effect &i_Effect, const char * i_path)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				eae6320::UserOutput::Print("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint vertexShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				const char* sourceCodeFileName = i_path;
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
				else if (vertexShaderId == 0)
				{
					wereThereErrors = true;
					eae6320::UserOutput::Print("OpenGL failed to get an unused vertex shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					eae6320::UserOutput::Print(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(vertexShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
							eae6320::UserOutput::Print(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						eae6320::UserOutput::Print(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(i_Effect.s_programId, vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
				goto OnExit;
			}
		}
		


	OnExit:

		if (vertexShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				eae6320::UserOutput::Print(errorMessage.str());
			}
			vertexShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}

}
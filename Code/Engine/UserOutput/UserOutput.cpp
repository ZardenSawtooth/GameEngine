#include "UserOutput.h"

void eae6320::UserOutput::Print(std::string i_String) {

	OutputDebugString(i_String.c_str());
	MessageBox(NULL, i_String.c_str(), NULL, MB_OK);
}
#pragma once
#include <WinBase.h>
#include <string>

namespace eae6320 {

	namespace UserOutput {

		void Print( std::string i_String) {

			OutputDebugString(i_String.c_str());
		}

	}

}
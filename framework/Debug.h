#ifndef __DEBUG_H__
#define __DEBUG_H__


/// @brief Utilities for debugging
namespace debug
{
	/// @brief Printing a message to stdout (and OutputDebugString on windows).
	void Printf(const char* fmt, ...);
	

}; // namespace debug

#endif // __DEBUG_H__

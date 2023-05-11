#include "Common.h"
#include <format>


#pragma region ---- print ----

void print(const char* fmt, const char* msg)
{
    if (msg == "") printf("%s\n", fmt);
    else printf(fmt, msg);
}

void printAndExit(const char* func, int exitCode)
{
    print("Failed to call %s()! exiting...", func);
    exit(exitCode);
}

std::string format(const double value)
{
	return std::format(" \033[1m\033[92m{:<6.1f}\033[0m", value);
}

#pragma endregion
